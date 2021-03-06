/*
	Remove um diretório. Novamente o caminho a ser fornecido é absoluto. Para que o diretório seja removido, ele
deve estar vazio, ou seja, não pode haver arquivos de nenhum tipo dentro dele. Se houver arquivos, a execução
do comando rmdirt2 deverá retornar uma mensagem de erro indicativa. Da mesma forma que no mkdirt2, se
algum dos diretórios fornecidos no string de caminho não existirem, o programa deverá retornar um mensagem
de erro. Exemplo de uso:
	%rmdirt2 /semestre/disciplina/inf01142
	
	Remove o diretório inf01142, se ele estiver vazio. Se os diretórios disciplina ou semestre não existirem
esse comando fornece uma mensagem de erro.
*/

#include "t2fs.h"
#include <stdio.h>

int clearDirectory(char* name); 

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("usage: %s </path/of/the/folder>\n", argv[0]);
		return 1;

	} else {
		char *name = argv[1];

		t2fs_file handle = t2fs_open(name);

		if (handle < 0) {
			printf("Incorrect folder name\n");
			return 1;
		}

		t2fs_close(handle);
               	
		int erro = removeDirectory(name);
		if (erro != 0) {
			printf("Deletion failed\n");
			return 1;
		}

		return 0;
	}
}

int removeDirectory(char* name) {
	t2fs_file handle = t2fs_open(name);

	struct t2fs_record record[4];
		
	t2fs_read(handle, (char*) record, 4*sizeof(struct t2fs_record));

	int i;

        int erro;
        while ((record[i].TypeVal == 1 || record[i].TypeVal == 2) && i < 4) {
                if (record[i].TypeVal == 2) {
                        erro = removeDirectory(record[i].name);

                } else {
                        erro = t2fs_delete(record[i].name);
                }
		
		i++;
        }

	t2fs_close(handle);

        return erro;
}
