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

int clearDirectory(t2fs_record *record) {
	int erro;
	while (record.TypeVal != TYPEVAL_INVALIDO) {
		if (record.TypeVal == TYPEVAL_DIRETORIO) {
			erro = clearDirectory(record);

		} else {
			erro = t2fs_delete(record.name);
		}
	}
	return erro;
}

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
	
		struct t2fs_record record;
		
		t2fs_read(handle, (char*) &record, sizeof(record));
		
		if (record.TypeVal == TYPEVAL_DIRETORIO) {
			int erro = clearDirectory(record);
			
			if (erro == 0) {
				t2fs_remove(name);
			} else {
				printf("Directory deletion failed\n");
				return 1;	
			}			

		} else {
			printf("Incorrect folder name\n");
			return 1;
		}

		return 0;
	}
}
