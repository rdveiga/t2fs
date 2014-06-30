/* 
    Realiza a cópia de um arquivo qualquer do sistema de arquivos nativo 
da máquina para o disco lógico do T2FS. O caminho de destino no disco 
T2FS é absoluto. Exemplo de uso 
%copy2t2 ./disciplina/inf01142/t2fs.pdf /aluno/sisop/trabalhopratico2.pdf

    Copia o arquivo t2fs.pdf, que se encontra no sistema de arquivos nativo 
da máquina (diretório disciplina, subdiretório inf01142), para o disco 
lógico T2FS, com o nome trabalhopratico2.pdf. Se os diretórios de destino, 
aluno ou sisop, não existirem esse comando fornece uma mensagem de erro.
*/

#include "t2fs.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	if (argc != 3) {
		printf("usage: %s </nativeDisk/file/path> </t2fsDisk/file/path>\n", argv[0]);
		return 1;
	} else {
		FILE *f = fopen(argv[1], "r");
		fseek(f, 0, SEEK_END);
		long fsize = ftell(f);
		fseek(f, 0, SEEK_SET);

		char *buffer = malloc(fsize + 1);
		fread(buffer, fsize, 1, f);
		fclose(f);

		buffer[fsize] = 0;

		t2fs_file handle = t2fs_create(argv[2]);
		int t2fsize = t2fs_write(handle, buffer, fsize);

		return 0;
	}
}
