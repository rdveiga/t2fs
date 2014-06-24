

#ifndef t2fs_file

/**************************************************
    ESSA ESTRUTURA DEVE SER COMPLETADA PELO ALUNO *
***************************************************/
typedef struct
{

	unsigned int rootBlockIdx; //número relativo do block de root relativo ao inicio da área de blocos de root
	unsigned int fileOffset; //indica qual o offset do arquivo no bloco de root	

} t2fs_find;

/**************************************
    NÃO ALTERAR ESSA PARTE DO ARQUIVO *
***************************************/
typedef int t2fs_file;

typedef struct
{
    unsigned char   name[40];
    unsigned int    blocksFileSize;
    unsigned int    bytesFileSize;
    unsigned int    dataPtr[2];
    unsigned int    singleIndPtr;
    unsigned int    doubleIndPtr;
} __attribute__((packed)) t2fs_record;

char *t2fs_identify(void);
t2fs_file t2fs_create (char *nome);
int t2fs_delete (char *nome);
t2fs_file t2fs_open (char *nome);
int t2fs_close (t2fs_file handle);
int t2fs_read (t2fs_file handle, char *buffer, int size);
int t2fs_write (t2fs_file handle, char *buffer, int size);
int t2fs_seek (t2fs_file handle, unsigned int offset);
int t2fs_first (t2fs_find *findStruct);
int t2fs_next (t2fs_find *findStruct, t2fs_record *dirFile);

#endif
