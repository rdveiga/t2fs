/*
	Criação de um diretório no T2FS. O caminho a ser fornecido é absoluto. Caso algum dos diretórios fornecidos
no string de caminho não existam, o programa deverá retornar um mensagem de erro. Exemplo de uso:
	%mkdirt2 /semestre/disciplina/inf01142
	
	Criar o diretório inf01142 dentro do diretório disciplina, que por sua vez, é um subdiretório de semestre.
Se os diretórios disciplina ou semestre não existirem esse comando fornece uma mensagem de erro.
*/

#include "t2fs.h"
#include <stdio.h>

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("usage: %s </path/of/the/folder>\n", argv[0]);
		return 1;

	} else {
		char *name = argv[1];

		t2fs_file handle = t2fs_create(name);

		if (handle < 0) {
			printf("Incorrect folder name\n");
			return 1;
		}
	
		struct t2fs_record record;
		
		t2fs_seek(handle, 0); // Posiçiona o contador de posições na posição 0
		t2fs_read(handle, (char*) &record, sizeof(record));

		record.TypeVal = TYPEVAL_DIRETORIO;

		t2fs_seek(handle, 0); // Posiçiona o contador de posições na posição 0
		t2fs_write(handle, (char*) &record, sizeof(record));

		t2fs_close(handle);

		return 0;
	}
}
