
//******************************************************************************
//*************** estrutura que implementa um descritor de arquivo *************
//******************************************************************************
typedef struct fileDescriptor_
{
	char name[40];
	char fileSizeInBlocks[4];
	char fileSizeInBytes[4];
	char directPointer0[4];
	char directPointer1[4];
	char simpleIndirectPtr[4];
	char doubleIndirectPtr[4];
	unsigned int rootBlock; //indica em qual bloco do root está o descritor do arquivo (não é o bloco absoluto do disco)
	unsigned int rootPos; //indica em qual posição do bloco do root está o descritor do arquivo

}FileDescriptor;


//******************************************************************************
//***************** estrutura de controle de um arquivo aberto******************
//******************************************************************************
typedef struct fileHandle_
{
	unsigned int handleNumber; //número do handle do arquivo aberto
	FileDescriptor* descriptor; //descritor do arquivo
	unsigned int filePointer;  //posição corrente no arquivo	

}FileHandle;

//******************************************************************************
//********************** estrutura de um nó da lista ***************************
//******************************************************************************

typedef struct Node_
{
	FileHandle* handle; //informações do arquivo aberto	
	struct Node_* NextNode;

}ListNode;


ListNode* initList();
void showNodes(ListNode* List);
ListNode* insert(ListNode* List, FileHandle* handle);
ListNode* search(ListNode* List, unsigned int handleNumber);
ListNode* removeNode(ListNode* List, unsigned int handleNumber);
ListNode* destroyList(ListNode* List);



