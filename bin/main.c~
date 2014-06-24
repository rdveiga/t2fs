#include <stdio.h>
#include "t2fs.h"


int main()
{
	//int a;
	//a = t2fs_open("");

	int handle;

	char buffer[1024];
	int a;

	for(a=0;a<1024;a++)
		buffer[a] = '\0';

	printf("\n\n%s\n\n",t2fs_identify());
/*
	handle = t2fs_open ("arq1");
	
	if(handle != -1)
		printf("\n\nFoi aberto um arquivo de handle: %d",handle);
	else
		printf("\n\nNao foi encontrado um arquivo com esse nome");

	if(t2fs_close(handle) == 0)
		printf("\n\nArquivo de handle %d foi fechado\n\n",handle);
	else
		printf("\n\nArquivo de handle %d nao se encontra aberto\n\n",handle);

	printf("\\sera testada a funcao de remover:\n\n");

	if(t2fs_delete ("arq1") == 0)
		printf("\n\nArquivo removido com sucesso");
	else
		printf("\n\nFalha ao remover arquivo\n\n");

	handle = t2fs_open ("arq1");
	
	if(handle != -1)
		printf("\n\nFoi aberto um arquivo de handle: %d\n\n",handle);
	else
		printf("\n\nNao foi encontrado um arquivo com esse nome\n\n");
*/	
	
	//t2fs_delete ("arq1");
	//t2fs_create("newArq1");
	t2fs_create("newArq2");
	t2fs_create("newArq3");
	//t2fs_create("newArq4");
	//t2fs_delete ("newArq1");
	//t2fs_delete ("newArq2");
	//t2fs_delete ("newArq3");
	//t2fs_delete ("newArq4");
	//t2fs_create("VaiPara");
	//t2fs_create("GrenobleINP");

	//t2fs_delete ("VaiPara");
	//t2fs_delete ("GrenobleINP");
	//t2fs_delete ("Henrique");
/*
	t2fs_create("Trab2sisop");
	t2fs_create("VouPassar");
	if(t2fs_delete("Trab2sisop") == 0)
		printf("\n\nTrab2Sisop removido\n\n");

*/

		handle = t2fs_open ("arq1");

		t2fs_seek(handle, 510);

		printf("\n\nForam escritos %d bytes no disco.\n\n", t2fs_write(handle,"bb",2));

		t2fs_seek(handle, 0);

		printf("\n\nForam lidos %d bytes no disco.\n\n",t2fs_read(handle, buffer, 512) );		
		printf("\n\nleitura: %s\n\n",buffer);

		/*

		handle = t2fs_open ("newArq1");
		//printf("\n\nForam escritos %d bytes no disco.\n\n", t2fs_write(handle,"Testando o sistema de arquivos",30));
		printf("\n\nForam lidos %d bytes no disco.\n\n",t2fs_read(handle, buffer, 512) );		
		printf("\n\nleitura: %s\n\n",buffer);


		for(a=0;a<1024;a++)
		buffer[a] = '\0';
		
		t2fs_seek(handle, 9);

		printf("\n\nForam lidos %d bytes no disco.\n\n",t2fs_read(handle, buffer, 100) );		
		printf("\n\nleitura: %s\n\n",buffer);


		t2fs_seek(handle, 9);

		printf("\n\nForam escritos %d bytes no disco.\n\n", t2fs_write(handle,"o programa mais legal ja desenvolvido por mim",45));

		t2fs_seek(handle, 0);

		for(a=0;a<1024;a++)
		buffer[a] = '\0';

		printf("\n\nForam lidos %d bytes no disco.\n\n",t2fs_read(handle, buffer, 100) );		
		printf("\n\nleitura: %s\n\n",buffer);

		for(a=0;a<1024;a++)
		buffer[a] = '\0';

		t2fs_seek(handle, 53);

		printf("\n\nForam lidos %d bytes no disco.\n\n",t2fs_read(handle, buffer, 100) );		
		printf("\n\nleitura: %s\n\n",buffer);


		for(a=0;a<1024;a++)
		buffer[a] = '\0';

		t2fs_seek(handle, 54);

		printf("\n\nForam lidos %d bytes no disco.\n\n",t2fs_read(handle, buffer, 100) );		
		printf("\n\nleitura: %s\n\n",buffer);

		
		t2fs_find* find = (t2fs_find*)malloc(sizeof(t2fs_find));

		t2fs_record* record = (t2fs_record*)malloc(sizeof(t2fs_record));

		t2fs_first(find);

		

		t2fs_next(find, record);
		
		printf("\n\nnome do arquivo: %s\n",record->name);
		printf("\ntamanho em blocos: %d\n",record->blocksFileSize);
		printf("\ntamanho em bytes: %d\n",record->bytesFileSize);
		printf("\nponteiro direto 0: %d\n",record->dataPtr[0]);
		printf("\nponteiro direto 1: %d\n",record->dataPtr[1]);

		t2fs_next(find, record);

		printf("\n\nnome do arquivo: %s\n",record->name);
		printf("\ntamanho em blocos: %d\n",record->blocksFileSize);
		printf("\ntamanho em bytes: %d\n",record->bytesFileSize);
		printf("\nponteiro direto 0: %d\n",record->dataPtr[0]);
		printf("\nponteiro direto 1: %d\n",record->dataPtr[1]);

		t2fs_next(find, record);

		printf("\n\nnome do arquivo: %s\n",record->name);
		printf("\ntamanho em blocos: %d\n",record->blocksFileSize);
		printf("\ntamanho em bytes: %d\n",record->bytesFileSize);
		printf("\nponteiro direto 0: %d\n",record->dataPtr[0]);
		printf("\nponteiro direto 1: %d\n",record->dataPtr[1]);

		t2fs_next(find, record);

		printf("\n\nnome do arquivo: %s\n",record->name);
		printf("\ntamanho em blocos: %d\n",record->blocksFileSize);
		printf("\ntamanho em bytes: %d\n",record->bytesFileSize);
		printf("\nponteiro direto 0: %d\n",record->dataPtr[0]);
		printf("\nponteiro direto 1: %d\n",record->dataPtr[1]);

*/



		//printf("\n\nForam escritos %d bytes no disco.\n\n", t2fs_write(handle, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 260) ); //35 chars
		//printf("\n\nForam escritos %d bytes no disco.\n\n", t2fs_write(handle, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 260) );
		//printf("\n\nForam escritos %d bytes no disco.\n\n", t2fs_write(handle, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 260) );

		//printf("\n\nForam lidos %d bytes no disco.\n\n",t2fs_read(handle, buffer, 512) );
		//printf("\n\nleitura: %s\n\n",buffer);
		//printf("\n\nForam lidos %d bytes no disco.\n\n",t2fs_read(handle, buffer, 1) );

		
	return 0;
}
