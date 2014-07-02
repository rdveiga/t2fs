/*
	Lista o conteúdo do diretório fornecido no caminho fornecido (absoluto). Se por um acaso não existir diretórios
que aparecem no meio do string de caminho, o programa dirt2 deverá retornar um mensagem de erro. A saída
desse programa é a listagem do conteúdo dos registros do arquivo de diretório formatado para apresentar, na
ordem, nome do arquivo, indicação se é um arquivo regular (r) ou diretório (d), a quantidade de blocos que
utiliza e o tamanho do arquivo em bytes. Exemplos de uso:
	%dirt2 /semestre/disciplina/inf01142
	Aula01.pdf	r	10	19500 bytes
	Atividade1	d	1	704 bytes
	Se o diretório não existir, uma mensagem de erro deverá ser gerada.
*/

#include "t2fs.h"
#include <stdio.h>

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

		struct t2fs_record record[4];

		t2fs_read(handle, (char*) record, 4*sizeof(struct t2fs_record));

    		int i;

    		while ((record[i].TypeVal == 1 || record[i].TypeVal == 2) && i < 4) {
        		printf("%s ", record[i].name);

        		if (record[i].TypeVal == 1)
            			printf("r ");
        		else
            			printf("d ");

        		printf("%d ", record[i].blocksFileSize);
        		printf("%d Bytes\n", record[i].bytesFileSize);
        		i++;
    		}

		t2fs_close(handle);

		return 0;
	}
}
