#include "t2fs.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("Invalid arguments");
		return !;
	} else {
		FILE *f = fopen(argv[0], "r");
		fseek(f, 0, SEEK_END);
		long fsize = ftell(f);
		fseek(f, 0, SEEK_SET);

		char *buffer = malloc(fsize + 1);
		fread(buffer, fsize, 1, f);
		fclose(f);

		buffer[fsize] = 0;

		t2fs_file handle = t2fs_create(argv[1]);
		int t2fsize = t2fs_write(handle, buffer, fsize);

		return 0;
	}
}
