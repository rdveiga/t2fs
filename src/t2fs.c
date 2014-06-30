#include <stdio.h>
#include <stdlib.h>
#include "apidisk.h"
#include "t2fs.h"
#include <string.h>
#include "list.h"


//********************************** defines auxiliares *************************************
#define desocupar 0
#define ocupar 1
//*******************************************************************************************


static unsigned int handle_ctrl = 0; //variável utilizada para controlar os handles criados


//*******************************************************************************************
//************************** Definição de um superbloco *************************************
//*******************************************************************************************

static unsigned int diskInitialized; //controla se as características do disco foram lidas

struct superblock_
{
	char id[4];
	unsigned int version;
	unsigned int ctrlSize;
	unsigned int diskSize;
	unsigned int blockSize;
	unsigned int freeBlockSize;
	unsigned int rootSize;
	unsigned int fileEntrySize;	
};


static struct superblock_ superblock; //instancia uma estrutura que armazena as informações do disco

ListNode* openedFiles;



//*******************************************************************************************
//*************************** funções para a manipulação dos blocos *************************
//*******************************************************************************************



//*******************************************************************************************
//função: littleEndianToInt()							            *
//                                                                                          *
//Descrição: converte uma sequencia de bytes no formato little endian para um inteiro       *
//										            *																				
//entradas: char* buffer: sequencia de bytes	                                            *
//          startPos: posição inicial do array	                                            *	
//	    int size: número de bytes que formam o numero em little endian                  *
//										            *
//saídas: Um inteiro representando a sequencia de bytes em little endian	            *
//*******************************************************************************************
unsigned int littleEndianToInt(unsigned char* buffer,int startPos, int size)
{
	unsigned int val;
	int idx;
f
	val = 0;

	val = buffer[startPos]; //posição inicial

	while(idx < size-1)
	{
		val = val + buffer[startPos + 1 + idx]*(256<<(idx*8) );
		idx ++;
	}

	return val;
}

//*******************************************************************************************
//função: intToLittleEndian()							            *
//                                                                                          *
//Descrição: converte um unsigned int em um array de bytes em little endian                 *
//										            *																				
//entradas: char* dstArray: array destino	                                            *
//          startPos: posição inicial do array	                                            *	
//										            *
//saídas: Um inteiro representando a sequencia de bytes em little endian	            *
//*******************************************************************************************
void intToLittleEndian(unsigned int val, unsigned char* dstArray)
{
	
	dstArray[3] =  (unsigned char)(val  >> 24   &   0x000000ff );
	dstArray[2] =  (unsigned char)((val >> 16)  &   0x000000ff);
	dstArray[1] =  (unsigned char)((val >> 8)   &   0x000000ff);
	dstArray[0] =  (unsigned char)( val         &   0x000000ff);

}

//*******************************************************************************************
//função: setupDiskInfo							                    *
//                                                                                          *
//Descrição: Carrega na estrutura superblock as características do disco                    *
//										            *																				
//entradas: nenhuma								            *
//										            *
//saídas: nenhuma					      			            *
//*******************************************************************************************

static unsigned int diskInitialized; //controla se as características do disco foram lidas

void setupDiskInfo()
{
	char setupBuffer[65536]; //tamanho máximo de um bloco

	if(diskInitialized == 0)
	{
		read_block(0, setupBuffer); //le o bloco zero

		superblock.id[0] = setupBuffer[0]; 
		superblock.id[1] = setupBuffer[1]; 
		superblock.id[2] = setupBuffer[2]; 
		superblock.id[3] = setupBuffer[3];
		superblock.version = setupBuffer[4];
		superblock.ctrlSize = setupBuffer[5];

		superblock.diskSize = littleEndianToInt(setupBuffer, 6, 4);
		superblock.blockSize = littleEndianToInt(setupBuffer, 10, 2);
		superblock.freeBlockSize = littleEndianToInt(setupBuffer, 12, 2);
		superblock.rootSize = littleEndianToInt(setupBuffer, 14, 2);
		superblock.fileEntrySize = littleEndianToInt(setupBuffer, 16, 2);

		//inicializa a lista de arquivos abertos
		openedFiles = initList();
	
		diskInitialized = 1; //flag que indica que não será mais necessário carregar o superbloco
		
	}
}

//*******************************************************************************************
//função: arrayCpy							                    *
//                                                                                          *
//Descrição: faz cópia de um array em outro de mesmo tamanho                    	    *
//										            *																				
//entradas: char* arrayDst: array que receberá a cópia 					    *
//          char* arrayOrigin: array que será copiado					    *
//	    int size: tamanho dos arrays 		    				    *
//										            *
//saídas: nenhuma					      			            *
//*******************************************************************************************
void arrayCpy(char* arrayDst, char* arrayOrigin, int size)
{
	int cpyIdx = 0;
	
	while(cpyIdx < size)
	{
		arrayDst[cpyIdx] = arrayOrigin[cpyIdx];
		cpyIdx ++;
	}
}

//*******************************************************************************************
//função: searchFreeBlock							            *
//                                                                                          *
//Descrição: procura no bitmap por um bloco livre                                           *
//										            *																				
//entradas: nenhuma								            *
//										            *
//saídas: Número de um bloco livre, caso exista.				            *
//	  -1 caso não há um bloco livre.					            *
//*******************************************************************************************
int searchFreeBlock()
{
	char blockBuffer[65536]; //tamanho máximo de um bloco
	char byteMasked; //variável utilizado para o bter o valor do bit desejado
	int  byteCounter; //número de bytes usados para representar os blocos
	int  byteAbsCnt; //indica o total de bytes varridos
	int  bitmapBlocks; //número de blocos da área de bitmap
	int  blockIdx;
	int  byteIdx;
	int  bitIdx;


	//calcula qual é o primeiro bloco (no disco) da área de dados	
	int firstDataBlock = superblock.ctrlSize + superblock.freeBlockSize + superblock.rootSize;

	//aponta para o bloco da área de bitmap que contém o bit que representa o primeiro bit da área de dados
	int bitmapBlock = (firstDataBlock/8)/superblock.blockSize;  //bloco do conjunto de blocos do bitmap
	int bitMapDiskBlock =  superblock.ctrlSize + bitmapBlock;  //bloco efetivo do disco a ser lido

	//calcula em qual byte e bit do bloco está sendo representado o status do bloco
	int bytePos = (firstDataBlock/8) - ( superblock.blockSize*bitmapBlock );
	int bitPos = 7 - ( ( firstDataBlock - (superblock.blockSize*8*bitmapBlock) ) - 8*bytePos);
		
	//indica o número de blocos da área de bitmap a serem varridos
	bitmapBlocks = superblock.freeBlockSize - bitmapBlock;
	

	for(blockIdx = 0; blockIdx < bitmapBlocks; blockIdx ++) //leitura dos blocos                               
	{	

		read_block(bitMapDiskBlock + blockIdx, blockBuffer); //efetua a leitura no disco 

		//verifica se está sendo analisado o primeiro bloco da área de dados
		if(blockIdx == 0)	
			byteIdx = bytePos;
		else
			byteIdx = 0;		
	
	
		for(byteIdx = 0; byteIdx < superblock.blockSize; byteIdx ++) //analisa os bytes de um bloco
		{
			//verifica se está sendo analisado o primeiro bloco da área de dados
			if(byteIdx == bytePos)	
				bitIdx = bitPos;
			else
				bitIdx = 7;			

			for(bitIdx = 7; bitIdx >= 0; bitIdx --)  //analisa os bits que representam os blocos
			{
				byteMasked = blockBuffer[byteIdx];
				byteMasked = byteMasked << (7-bitIdx);
				byteMasked = byteMasked & 128;
				if(byteMasked == 0)
					return ( blockIdx*superblock.blockSize*8 + 8*byteIdx + (7-bitIdx) );
			}
		}
	}

	return -1; //caso não há nenhum bloco livre
}

//*******************************************************************************************
//função: writeBlockBitMap							            *
//                                                                                          *
//Descrição: Escreve no bitmap se o bloco está livre/ocupado                                *
//										            *																				
//entradas: bloco: número do bloco a ter seu status modificado			            *
//	    status: 0:livre, 1:ocupado						            *
//										            *
//saídas:  nenhuma				                                            *
//*******************************************************************************************
void writeBlockBitmap(int bloco, int status)
{
	char blockBuffer[65536]; //tamanho máximo de um bloco
	int byteIdx;
	int bitIdx;
	char byteMasked;
	
	//aponta para o bloco da área de bitmap => superbloco|bitmap|
	int bitmapBlock = (bloco/8)/superblock.blockSize;  //bloco do conjunto de blocos do bitmap
	int diskBlock =  superblock.ctrlSize + bitmapBlock; //bloco efetivo do disco

	//calcula em qual byte e bit do bloco está sendo representado o status do bloco
	byteIdx = (bloco/8) - ( superblock.blockSize*bitmapBlock );
	bitIdx = 7-( ( bloco - (superblock.blockSize*8*bitmapBlock) ) - 8*byteIdx);
	
	read_block(diskBlock, blockBuffer); //efetua a leitura do bloco de bitmap	                 

	byteMasked = blockBuffer[byteIdx];

		
	if(status == desocupar) //setar o bloco como livre

		byteMasked = byteMasked & ( ~ ( 1 << bitIdx) );

	else //setar o bloco como ocupado
	
		byteMasked = byteMasked | ( 1 << bitIdx);
	
	blockBuffer[byteIdx] = byteMasked;

	write_block(diskBlock, blockBuffer);                                            
}

//*******************************************************************************************
//função: searchFile							                    *
//                                                                                          *
//Descrição: Procura um determinado arquivo no disco               		            *
//										            *																				
//entradas: nome do arquivo							            *
//										            *
//saídas: descritor com as informações do arquivo, caso ele exista		            *
//	  null, caso o arquivo não exista                                                   *
//*******************************************************************************************
FileDescriptor* searchFile(char *name)
{
	char blockBuffer[65536]; //tamanho máximo de um bloco
	char fileReg[64]; 	 //buffer para o registro de um arquivo
	char valChar;	
	char fileName[40]; 	 //uma posição a mais para o \0
	int  firstRootBlock;
	int  cpyIdx;
	int  numFiles; 		 //indica o número de arquivos que um bloco de diretório pode endereçar
	int  blockIdx;
	int  fileIdx;
	FileDescriptor* openedFile; //descritor do arquivo a ser aberto
	

	//calcula o primeiro bloco do root
	firstRootBlock = superblock.ctrlSize + superblock.freeBlockSize;

	//calcula o número máximo de arquivos por bloco de diretório
	numFiles = superblock.blockSize/64;

	for(blockIdx = 0; blockIdx < superblock.rootSize; blockIdx ++) //varre os blocos que compoem o diretório
	{

		read_block(firstRootBlock + blockIdx, blockBuffer);
		
		for(fileIdx = 0; fileIdx < numFiles; fileIdx ++) //varre todos os registros de arquivos de um bloco de diretório
		{

			for(cpyIdx = 0; cpyIdx < 64; cpyIdx ++ ) //faz a leitura de um registro
			{
				fileReg[cpyIdx] = blockBuffer[cpyIdx + 64*fileIdx];	
			}

			//analisa o registro para verificar se é o arquivo desejado
			
			valChar = fileReg[1];

			if(valChar!=0xFF) //caso seja um arquivo válido, verifica o restante dos caracteres
			{
				int nameCnt = 1;

				while( (nameCnt < 40) && (fileReg[nameCnt] != 0) ) //copia o nome para uma string !!!!cuidado!!!!
				{
					fileName[nameCnt-1] = fileReg[nameCnt];
					nameCnt ++;
				}
				fileName[nameCnt] = '\0';

				if(strcmp(name,fileName) == 0)
				{
					openedFile = (FileDescriptor*)malloc(sizeof(FileDescriptor));
					arrayCpy(openedFile->typeVal, &fileReg[0], 1);
					arrayCpy(openedFile->name, &fileReg[1], 39); // no nosso é apartir do 1
					arrayCpy(openedFile->fileSizeInBlocks, &fileReg[40], 4);
					arrayCpy(openedFile->fileSizeInBytes, &fileReg[44], 4);
					arrayCpy(openedFile->directPointer0, &fileReg[48], 4);
					arrayCpy(openedFile->directPointer1, &fileReg[52], 4);
					arrayCpy(openedFile->simpleIndirectPtr, &fileReg[56], 4);
					arrayCpy(openedFile->doubleIndirectPtr, &fileReg[60], 4);
					openedFile->rootBlock = blockIdx;
					openedFile->rootPos = fileIdx;
				
					return openedFile; //retorna um descritor para o arquivo encontrado	
				}
			}	
		}
	}

	return NULL; //caso não encontrou o arquivo
}

//*******************************************************************************************
//função: updateFile							                    *
//                                                                                          *
//Descrição: Atualiza as informações do arquivo no root              		            *
//										            *																				
//entradas: número do handle do arquivo aberto						    *
//										            *
//saídas: nenhuma		                                                            *
//	                                                   				    *
//*******************************************************************************************
void updateFile(unsigned int handleNumber)
{
	char blockBuffer[65536]; //tamanho máximo de um bloco
	unsigned int firstRootBlock;
	unsigned int diskRootBlock;
	unsigned int filePos;
	ListNode* node;
	FileDescriptor* openedFile;

	//carrega o descritor do arquivo de handle fornecido
	node = search(openedFiles,handleNumber);

	if(node != NULL)
	{
         
		//calcula o bloco do root a ser carregado
		firstRootBlock = superblock.ctrlSize + superblock.freeBlockSize; //calcula o primeiro bloco do root

		diskRootBlock = firstRootBlock + node->handle->descriptor->rootBlock; //calcula qual é o bloco efetivo onde está o registro

		read_block(diskRootBlock, blockBuffer);	//carrega o bloco do root onde está o registro do arquivo

		filePos = node->handle->descriptor->rootPos; //indica qual é a posição no bloco de root do arquivo

		blockBuffer[64*filePos + 40] = node->handle->descriptor->fileSizeInBlocks[0];
		blockBuffer[64*filePos + 41] = node->handle->descriptor->fileSizeInBlocks[1];
		blockBuffer[64*filePos + 42] = node->handle->descriptor->fileSizeInBlocks[2];
		blockBuffer[64*filePos + 43] = node->handle->descriptor->fileSizeInBlocks[3];

		blockBuffer[64*filePos + 44] = node->handle->descriptor->fileSizeInBytes[0];
		blockBuffer[64*filePos + 45] = node->handle->descriptor->fileSizeInBytes[1];
		blockBuffer[64*filePos + 46] = node->handle->descriptor->fileSizeInBytes[2];
		blockBuffer[64*filePos + 47] = node->handle->descriptor->fileSizeInBytes[3];

		blockBuffer[64*filePos + 48] = node->handle->descriptor->directPointer0[0];
		blockBuffer[64*filePos + 49] = node->handle->descriptor->directPointer0[1];
		blockBuffer[64*filePos + 50] = node->handle->descriptor->directPointer0[2];
		blockBuffer[64*filePos + 51] = node->handle->descriptor->directPointer0[3];

		blockBuffer[64*filePos + 52] = node->handle->descriptor->directPointer1[0];
		blockBuffer[64*filePos + 53] = node->handle->descriptor->directPointer1[1];
		blockBuffer[64*filePos + 54] = node->handle->descriptor->directPointer1[2];
		blockBuffer[64*filePos + 55] = node->handle->descriptor->directPointer1[3];

		blockBuffer[64*filePos + 56] = node->handle->descriptor->simpleIndirectPtr[0];
		blockBuffer[64*filePos + 57] = node->handle->descriptor->simpleIndirectPtr[1];
		blockBuffer[64*filePos + 58] = node->handle->descriptor->simpleIndirectPtr[2];
		blockBuffer[64*filePos + 59] = node->handle->descriptor->simpleIndirectPtr[3];

		blockBuffer[64*filePos + 60] = node->handle->descriptor->doubleIndirectPtr[0];
		blockBuffer[64*filePos + 61] = node->handle->descriptor->doubleIndirectPtr[1];
		blockBuffer[64*filePos + 62] = node->handle->descriptor->doubleIndirectPtr[2];
		blockBuffer[64*filePos + 63] = node->handle->descriptor->doubleIndirectPtr[3];

		write_block(diskRootBlock, blockBuffer); //atualiza o bloco		
	}
}

//*******************************************************************************************
//função: fileSizeInfo							                    *
//                                                                                          *
//Descrição: Imprime o tamanho do arquivo (em blocos e bytes)  (função para depuração)      *
//										            *																				
//entradas: nome do arquivo						    		    *
//										            *
//saídas: nenhuma		                                                            *
//	                                                   				    *
//*******************************************************************************************
void fileSizeInfo(char* name)
{
	FileDescriptor* descriptor;
	unsigned int size;
	descriptor = searchFile(name);

	if(descriptor != NULL)
	{
		size = littleEndianToInt(descriptor->fileSizeInBlocks,0, 4);
		printf("\n\nTamanho em blocos: %d",size);

		size = littleEndianToInt(descriptor->fileSizeInBytes,0, 4);
		printf("\nTamanho em bytes: %d\n\n",size);

		free(descriptor);
	}
}

//*******************************************************************************************
//função: newFile							                    *
//                                                                                          *
//Descrição: aloca no root um registro de um novo arquivo. Aloca um bloco de dados, setando *
//	     o primeiro ponteiro de índice. Zera os outros ponteira de índice.              *
//										            *																				
//entradas: nome do arquivo							            *
//										            *
//saídas: 0, se o novo registro foi inserido no diretório                                   *
//        -1, em caso de alguma falha                                                       *
//*******************************************************************************************
int newFile(char* name)
{
	char blockBuffer[65536]; //tamanho máximo de um bloco
	char newFileReg[64];
	int diskRootBlock;
	int numFiles;
	int blockIdx;
	int fileIdx;
	char valChar;
	int cnt;
	unsigned int newDataBlock;

	//**************************************
	//procura no root por uma posição livre:
	//**************************************

	//calcula o número máximo de arquivos por bloco de diretório
	numFiles = superblock.blockSize/64;
	
	diskRootBlock = superblock.ctrlSize + superblock.freeBlockSize; //calcula o bloco inicial do root no disco

	for(blockIdx = 0; blockIdx < superblock.rootSize; blockIdx ++) //varre os blocos que compoem o diretório
	{
		
		read_block(diskRootBlock + blockIdx, blockBuffer);

		for(fileIdx = 0; fileIdx < numFiles; fileIdx ++) //varre todos os registros de arquivos de um bloco de diretório
		{
			valChar = blockBuffer[64*fileIdx];
			
			if(valChar == 0xFF) //se encontrou um registro inválido
			{

				//*************************************************
				//a partir daqui, adiciona um novo registro no root
				//*************************************************

				//inicializa o novo registro
				for(cnt = 0; cnt < 64; cnt ++)
					blockBuffer[cnt + 64*fileIdx] = 0;
			

				//SETA O 1º caractere de validade
				blockBuffer[fileIdx*64] = 0x01; //VERIFICAR!!

				// copia o nome do aqruivo para o blockBuffer
				cnt = 0;

				while( name[cnt] != '\0' )
				{
					blockBuffer[cnt + (64*fileIdx +1)] = name[cnt]; //NAO SEI SE TA CERTO!!!
					cnt ++;
				}



				//aloca um bloco de dados para o arquivo novo
				
				newDataBlock = searchFreeBlock(); //procura por um bloco livre na área de dados

				writeBlockBitmap(newDataBlock, ocupar); //indica no bitmap que o bloco está agora ocupado

				intToLittleEndian(1, &blockBuffer[64*fileIdx + 40]); //atualiza o tamanho em blocos

				intToLittleEndian(newDataBlock, &blockBuffer[64*fileIdx + 48]); //atualiza o ponteiro direto 0
			
				write_block(diskRootBlock + blockIdx, blockBuffer); //escreve o registro no root

				return 0;
			}		
		}
	}

	return -1;	
}

//*******************************************************************************************
//função: removeFromRoot							            *
//                                                                                          *
//Descrição: seta o arquivo no diretório como inválido               		            *
//										            *																				
//entradas: nome do arquivo							            *
//										            *
//saídas: 0, se o arquivo foi removido do diretório                                         *
//        -1, em caso de alguma falha                                                       *
//*******************************************************************************************
int removeFromRoot(char* name)
{
	char blockBuffer[65536]; //tamanho máximo de um bloco
	FileDescriptor* descriptor;
	int diskBlock;

	descriptor = searchFile(name); //procura o arquivo no root

	if(descriptor != NULL)
	{
		
		diskBlock = superblock.ctrlSize + superblock.freeBlockSize + descriptor->rootBlock; //calcula o bloco de disco do root

		read_block(diskBlock, blockBuffer);

		blockBuffer[descriptor->rootPos*64] = descriptor->name[0] & 127; //força o bit 7 do primeiro caracter do nome para "0"

		write_block(diskBlock, blockBuffer); //escreve de volta o bloco

		return 0;
	}
}

//*******************************************************************************************
//função: removeFile							                    *
//                                                                                          *
//Descrição: Remove um determinado arquivo no disco. Seta no bitmao como "0" os blocos que  *
//	     estavam sendo usados pelo arquivo.               		            	    *
//										            *																				
//entradas: nome do arquivo							            *
//										            *
//saídas: 0, se o arquivo foi removido                                                      *
//        -1, em caso de alguma falha                                                       *
//*******************************************************************************************
int removeFile(char *name)
{
	char blockBuffer0[65536]; //tamanho máximo de um bloco
	char blockBuffer1[65536]; //tamanho máximo de um bloco
	FileDescriptor* descriptor;
	unsigned int pointer;
	unsigned int indirectPtr0;
	unsigned int indirectPtr1;
	unsigned int numIdx; //número de indices de um bloco de indices
	int idx0; //utilizado nos laços para varrer os indices
	int idx1; //utilizado nos laços para varrer os indices
	int scan0; //variavel utilizada para determinar quando parar de varrer um bloco de indices
	int scan1; //variavel utilizada para determinar quando parar de varrer um bloco de indices	

	descriptor = searchFile(name); //procura pelo arquivo a ser removido no diretório

	if(descriptor != NULL) //se o arquivo foi encontrado no diretório e é válido
	{

		//calcula o número de indices que um bloco pode ter
		numIdx = superblock.blockSize/4;

		
		//*******************************************************************
		//obtem o primeiro ponteiro direto, liberando o bloco correspondente
		//*******************************************************************
		pointer = littleEndianToInt(descriptor->directPointer0,0, 4);
		if(pointer != 0) 
			writeBlockBitmap(pointer, desocupar);

		//******************************************************************
		//obtem o segundo ponteiro direto, liberando o bloco correspondente
		//******************************************************************
		pointer = littleEndianToInt(descriptor->directPointer1,0, 4);
		if(pointer != 0) 
			writeBlockBitmap(pointer, desocupar);

		//**********************************************************************
		//obtem o ponteiro de indireção simples, liberando os blocos associados
		//**********************************************************************
		pointer = littleEndianToInt(descriptor->simpleIndirectPtr,0, 4);

		if(pointer != 0) //se há um bloco de índices alocado
		{
			read_block(pointer, blockBuffer0); //carrega o bloco de indices
		
			idx0 = 0;
			scan0 = 1;
			
			while( (idx0 < numIdx) && (scan0 == 1) )
			{
				indirectPtr0 = littleEndianToInt(blockBuffer0,idx0*4, 4);
			
				if(indirectPtr0 != 0)			
					writeBlockBitmap(indirectPtr0, desocupar);
				else
					scan0 = 0;
				idx0 ++;
			}
			writeBlockBitmap(pointer, desocupar);//após ter varrido o bloco de indices, ele é liberado
		}

		//********************************************************************
		//obtem o ponteiro de indireção dupla, liberando os blocos associados
		//********************************************************************
		pointer = littleEndianToInt(descriptor->doubleIndirectPtr,0, 4); //aponta para o bloco de indice nivel 1

		if(pointer !=0) //se há um bloco de índice de nivel 1 alocado
		{

			read_block(pointer, blockBuffer0); //carrega o bloco de indice nivel 1
		
			idx0 = 0;
			scan0 = 1;

			while( (idx0 < numIdx) && (scan0 == 1) ) 
			{
				indirectPtr0 = littleEndianToInt(blockBuffer0,idx0*4, 4); //busca o bloco de indice nivel 2
			
				if(indirectPtr0 != 0) //se há um bloco de nivel 2 alocado			
				{
					read_block(pointer, blockBuffer1); //carrega o bloco de indice nivel 2

					idx1 = 0;
					scan1 = 1;

					while( (idx1 < numIdx) && (scan1 == 1) ) 
					{
						indirectPtr1 = littleEndianToInt(blockBuffer1,idx1*4, 4); //busca os blocos de dados
						
						if(indirectPtr1 != 0)			
							writeBlockBitmap(indirectPtr0, desocupar); //libera o bloco de dados
						else
							scan1 = 0;
						idx1 ++;
					}	
					writeBlockBitmap(indirectPtr0, desocupar);//após ter varrido o bloco de nivel 2 ele é liberado
				}
				else
					scan0 = 0;
				idx0 ++;
			}
			writeBlockBitmap(pointer, desocupar);//após ter varrido o bloco de nivel 1 ele é liberado		
		}
		
		//depois que todos os blocos de indices e dados foram liberados, seta o bit 7 do 
		//primeiro caracter do nome como 0
	
		removeFromRoot(name);
		
		return 0;
	}
	return -1;
}




//*******************************************************************************************
//*************************** funções de interface com o usuário ****************************
//*******************************************************************************************


char *t2fs_identify(void)
{

	return "Arthur Ribero - 207834\nRonaldo Dall'Agnol Veiga - 209823\nGabriel St-Denis - 247170";
}


int t2fs_delete(char *nome)
{
	setupDiskInfo();

	return removeFile(nome);
}


t2fs_file t2fs_open(char *nome)
{
	FileHandle* handle; //handle para um arquivo aberto
	FileDescriptor* descriptor; //descritor para um arquivo aberto

	setupDiskInfo(); //verifica se as informações do disco foram obtidas
	
	descriptor = searchFile(nome); //procura no disco por um arquivo com o nome fornecido
	
	if(descriptor != NULL) //se o arquivo foi encontrado
	{
		handle = (FileHandle*)malloc(sizeof(FileHandle)); //cria um handle
		handle->handleNumber = handle_ctrl;
		handle_ctrl ++; //atualiza a variavel global que controla o numero do ultimo handle criado
		handle->descriptor = descriptor;
		handle->filePointer = 0; //aponta para o inicio do arquivo
		
		openedFiles = insert(openedFiles, handle); //insere o handle do arquivo aberto na lista de arquivos abertos

		return handle->handleNumber;	
	}

	return -1; //caso o arquivo não foi encontrado
}


t2fs_file t2fs_create(char *nome)
{
	FileDescriptor* descriptor;
	
	setupDiskInfo(); //verifica se as informações do disco foram obtidas

	descriptor = searchFile(nome); //procura no disco por um arquivo com o nome fornecido

	//verifica se ja existe um arquivo com o nome fornecido. Se existe, remove-o.
	if(descriptor != NULL)
		removeFile(nome);	

	if( newFile(nome) == 0 )
		return t2fs_open(nome);
	else
		return -1;	
}	


int t2fs_close(t2fs_file handle)
{
	setupDiskInfo(); //verifica se as informações do disco foram obtidas

	if(search(openedFiles, handle) != NULL) //verifica se existe algum arquivo aberto com o handle fornecido
	{
		openedFiles = removeNode(openedFiles, handle);
		return 0;	
	}
	return -1;
}


int t2fs_write(t2fs_file handle, char *buffer, int size)
{	
	FileHandle* handleFile;
	ListNode* node;
	char blockBuffer[65536]; //tamanho máximo de um bloco
	unsigned int numBlock;
	unsigned int blockIdx; //controla a posição dentro do bloco
	unsigned int bufferIdx; //cotrola a posição de varredura do buffer de entrada
	unsigned int idx; //controla se todos os bytes foram transferidos para o disco

	unsigned int sizeInBytes;
	unsigned int sizeInBlocks;

	unsigned int newDataBlock;
 
	setupDiskInfo(); //verifica se as informações do disco foram obtidas

	node = search(openedFiles, handle);

	if(node != NULL) //se o arquivo está aberto
	{

		handleFile = node->handle; //captura o handle do arquivo aberto

		idx = size; //seta o número de bytes a serem transferidos para o disco

		bufferIdx = 0; //aponta para o inicio do buffer de entrada

		while(idx > 0)	//controla se a transferencia do buffer de entrada para o disco foi concluida	
		{

			if(handleFile->filePointer < superblock.blockSize) //ponteiro direto 0
			{
				numBlock = littleEndianToInt(handleFile->descriptor->directPointer0, 0, 4); //bloco do disco

				read_block(numBlock, blockBuffer); //carrega o bloco do disco
			
				blockIdx = handleFile->filePointer; //local do bloco que será escrito

				while( (blockIdx < superblock.blockSize) && (idx > 0) ) 
				{
					blockBuffer[blockIdx] = buffer[bufferIdx];
					blockIdx++;
					bufferIdx++;
					idx--;
					handleFile->filePointer++; //atualiza o ponteiro de arquivo

					//atualiza o tamanho do arquivo
					sizeInBytes = littleEndianToInt(handleFile->descriptor->fileSizeInBytes,0, 4);

					if((handleFile->filePointer) > sizeInBytes)
						intToLittleEndian(handleFile->filePointer, handleFile->descriptor->fileSizeInBytes);		
				}
				write_block(numBlock, blockBuffer); //escreve de volta no disco o bloco

			}
			else if(handleFile->filePointer < 2*superblock.blockSize) //ponteiro direto 1
			{
				numBlock = littleEndianToInt(handleFile->descriptor->directPointer1, 0, 4); //bloco do disco

				if(numBlock == 0) //caso não há um segundo bloco alocado
				{

					newDataBlock = searchFreeBlock(); //procura por um bloco livre na área de dados

					writeBlockBitmap(newDataBlock, ocupar); //indica no bitmap que o bloco está agora ocupado

					intToLittleEndian(newDataBlock, handleFile->descriptor->directPointer1); //atualiza o ponteiro direto 1

					numBlock = newDataBlock;
				}

				read_block(numBlock, blockBuffer); //carrega o bloco do disco

				blockIdx = handleFile->filePointer - superblock.blockSize; //local do bloco que será escrito

				while( (blockIdx < superblock.blockSize) && (idx > 0) ) 
				{
					blockBuffer[blockIdx] = buffer[bufferIdx];
					blockIdx++;
					bufferIdx++;
					idx--;
					handleFile->filePointer ++; //atualiza o ponteiro de arquivo
					
					//atualiza o tamanho do arquivo
					sizeInBytes = littleEndianToInt(handleFile->descriptor->fileSizeInBytes,0, 4);

					if((handleFile->filePointer) > sizeInBytes)
						intToLittleEndian(handleFile->filePointer, handleFile->descriptor->fileSizeInBytes);
				}

				sizeInBlocks = littleEndianToInt(handleFile->descriptor->fileSizeInBlocks,0, 4);
				if( 2 > sizeInBlocks )
					intToLittleEndian(2, handleFile->descriptor->fileSizeInBlocks);
			
				write_block(numBlock, blockBuffer); //escreve de volta no disco o bloco	
			}
			else //indireção simples e dupla nao implementada
			{
				printf("\n\nNao foi implementado o acesso a blocos de dados atraves de blocos de indice.\n\n");
				updateFile(handle); //atualiza o registro do arquivo no root
				return size - idx;
			}

		}//fim while principal
		
		updateFile(handle); //atualiza o registro do arquivo no root
		return size - idx;
	}
	else
		return -1; //caso o arquivo não está aberto

}//fim da função write



int t2fs_read(t2fs_file handle, char *buffer, int size)
{
	FileHandle* handleFile;
	ListNode* node;
	char blockBuffer[65536]; //tamanho máximo de um bloco
	unsigned int numBlock;
	unsigned int blockIdx; //controla a posição dentro do bloco
	unsigned int bufferIdx; //cotrola a posição de varredura do buffer de entrada
	unsigned int readBytes; //controla o numero de bytes lidos
	unsigned int fileSize; //indica o tamanho do arquivo em bytes
 
	setupDiskInfo(); //verifica se as informações do disco foram obtidas

	node = search(openedFiles, handle);

	if(node != NULL) //se o arquivo está aberto
	{

		handleFile = node->handle; //captura o handle do arquivo aberto

		fileSize = littleEndianToInt(node->handle->descriptor->fileSizeInBytes,0, 4); //numero de bytes do arquivo

		bufferIdx = 0; //aponta para o inicio do buffer de saída

		readBytes = 0; //seta o numero de bytes lidos

		while( (readBytes < size) && (handleFile->filePointer < fileSize) ) //controla a leitura	
		{

			if(handleFile->filePointer < superblock.blockSize) //ponteiro direto 0
			{
				numBlock = littleEndianToInt(handleFile->descriptor->directPointer0, 0, 4); //bloco do disco

				read_block(numBlock, blockBuffer); //carrega o bloco do disco
			
				blockIdx = handleFile->filePointer; //local do bloco onde começa a leitura

				while( (blockIdx < superblock.blockSize) && (readBytes < size) && (handleFile->filePointer < fileSize) ) 
				{
					buffer[bufferIdx] = blockBuffer[blockIdx];
					blockIdx++;
					bufferIdx++;
					handleFile->filePointer++; //atualiza o ponteiro de arquivo
					readBytes ++;		
				}
			}
			else if(handleFile->filePointer < 2*superblock.blockSize) //ponteiro direto 1
			{
				numBlock = littleEndianToInt(handleFile->descriptor->directPointer1, 0, 4); //bloco do disco

				read_block(numBlock, blockBuffer); //carrega o bloco do disco

				blockIdx = handleFile->filePointer - superblock.blockSize; //local do bloco onde começa a leitura

				while( (blockIdx < superblock.blockSize) && (readBytes < size) && (handleFile->filePointer < fileSize) ) 
				{
					buffer[bufferIdx] = blockBuffer[blockIdx];
					blockIdx++;
					bufferIdx++;
					handleFile->filePointer ++; //atualiza o ponteiro de arquivo
					readBytes ++;	
				}
			}
			else //indireção simples e dupla nao implementada
			{
				printf("\n\nNao foi implementado o acesso a blocos de dados atraves de blocos de indice.\n\n");
				return readBytes;
			}

		}//fim while principal
	
		return readBytes;
	}
	else
		return -1; //caso o arquivo não está aberto

	
}

int t2fs_seek(t2fs_file handle, unsigned int offset)
{
	
	FileHandle* handleFile;
	ListNode* node;
	unsigned int fileSize; //tamanho do arquivo em bytes
	
	setupDiskInfo(); //verifica se as informações do disco foram obtidas

	node = search(openedFiles, handle);

	if(node != NULL) //se há um arquivo aberto com o handle fornecido
	{
		handleFile = node->handle;

		fileSize = littleEndianToInt(handleFile->descriptor->fileSizeInBytes, 0, 4);

		if(offset < fileSize)
		{
			handleFile->filePointer = offset;
			return 0;
		}		
	}

	return -1; //em caso de erro
}
/*
int t2fs_first(t2fs_find *findStruct)
{
	setupDiskInfo(); //verifica se as informações do disco foram obtidas

	char blockBuffer[65536]; //tamanho máximo de um bloco
	unsigned int  firstRootBlock;
	unsigned int  numFiles; 	
	unsigned int  blockIdx;
	unsigned int  fileIdx;
	char firstNameChar;
	

	//calcula o primeiro bloco do root
	firstRootBlock = superblock.ctrlSize + superblock.freeBlockSize;

	//calcula o número máximo de arquivos por bloco de diretório
	numFiles = superblock.blockSize/64;

	for(blockIdx = 0; blockIdx < superblock.rootSize; blockIdx ++) //varre os blocos que compoem o diretório
	{
		read_block(firstRootBlock + blockIdx, blockBuffer);
		
		for(fileIdx = 0; fileIdx < numFiles; fileIdx ++) //varre todos os registros de arquivos de um bloco de diretório
		{

			//verifica se é o registro de um arquivo válido, verificando
			//se o bit 7 do primeiro caractere está em "1"

			firstNameChar = blockBuffer[64*fileIdx];
			firstNameChar = firstNameChar & 128; 

			if(firstNameChar) //caso seja um arquivo válido
			{
				findStruct->rootBlockIdx = blockIdx;
				findStruct->fileOffset = fileIdx;

				return 0;		
			}	
		}
	}

	return -1; //caso não há arquivos no diretório	
}

int t2fs_next(t2fs_find *findStruct, t2fs_record *dirFile)
{
	setupDiskInfo(); //verifica se as informações do disco foram obtidas

	char blockBuffer[65536]; //tamanho máximo de um bloco
	unsigned int firstRootBlock;
	unsigned int numFiles; 	
	unsigned int blockIdx;
	unsigned int fileIdx;
	unsigned int nextBlockIdx;
	unsigned int nextFileIdx;
	char firstNameChar;
	

	//calcula o primeiro bloco do root
	firstRootBlock = superblock.ctrlSize + superblock.freeBlockSize;

	//calcula o número máximo de arquivos por bloco de diretório
	numFiles = superblock.blockSize/64;



	//obtém o registro do arquivo
	
	read_block(firstRootBlock + findStruct->rootBlockIdx, blockBuffer);  

	arrayCpy(dirFile->name, &blockBuffer[findStruct->fileOffset*64], 40);
    	dirFile->blocksFileSize = littleEndianToInt(blockBuffer,40 + findStruct->fileOffset*64, 4);
    	dirFile->bytesFileSize = littleEndianToInt(blockBuffer,44 + findStruct->fileOffset*64, 4);
    	dirFile->dataPtr[0] = littleEndianToInt(blockBuffer,48 + findStruct->fileOffset*64, 4);
	dirFile->dataPtr[1] = littleEndianToInt(blockBuffer,52 + findStruct->fileOffset*64, 4);
    	dirFile->singleIndPtr = littleEndianToInt(blockBuffer,56 + findStruct->fileOffset*64, 4);
    	dirFile->doubleIndPtr = littleEndianToInt(blockBuffer,60 + findStruct->fileOffset*64, 4);

	//*******************************************
	//************ obtém o próximo registro *****
	//*******************************************

	nextFileIdx =  findStruct->fileOffset + 1;
	nextBlockIdx = findStruct->rootBlockIdx;

	if( nextFileIdx > numFiles) //se o próximo registro está no bloco seguinte
	{
		nextFileIdx = 0;
		nextBlockIdx ++;
	}
	
	for(blockIdx = nextBlockIdx; blockIdx < superblock.rootSize; blockIdx ++) //varre os blocos que compoem o diretório
	{
		read_block(firstRootBlock + blockIdx, blockBuffer);
		
		for(fileIdx = nextFileIdx; fileIdx < numFiles; fileIdx ++) //varre todos os registros de arquivos de um bloco de diretório
		{
			
			
			//verifica se é o registro de um arquivo válido, verificando
			//se o bit 7 do primeiro caractere está em "1"

			firstNameChar = blockBuffer[64*fileIdx];
			firstNameChar = firstNameChar & 128; 

			if(firstNameChar) //caso seja um arquivo válido
			{
				findStruct->rootBlockIdx = blockIdx;
				findStruct->fileOffset = fileIdx;

				return 0;		
			}	
		}
	}

	return -1; //caso não há mais arquivos no diretório	
}
*/


