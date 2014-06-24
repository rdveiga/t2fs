

#ifndef T2FS_H
#define T2FS_H

typedef unsigned char BYTE;
typedef unsigned short int WORD;
typedef unsigned int DWORD;
typedef int BOOL;
#define FALSE 0
#define TRUE 1

typedef int t2fs_file;

#define TYPEVAL_REGULAR     0x01
#define TYPEVAL_DIRETORIO   0x02
#define TYPEVAL_INVALIDO    0xFF    // qualquer outro valor tamb�m � invalido

/** Registro de diret�rio (entrada de diret�rio) */
struct t2fs_record {
    /* Tipo da entrada. Indica se o registro � v�lido e, se for, o tipo do arquivo (regular ou diret�rio).
    �	0xFF, registro inv�lido (n�o associado a nenhum arquivos);
    �	0x01 arquivo regular;
    �	0x02, arquivo de diret�rio.
    */
    BYTE    TypeVal;        //   0:  1 byte

    /* Nome do arquivo. : string com caracteres ASCII (0x21 at� 0x7A), case sensitive.
    O string deve terminar com o caractere especial �\0� (0x00). */
    char    name[39];       //   1: 39 bytes

    /* Tamanho do arquivo. Expresso, apenas, em n�mero de blocos de dados
    (n�o est�o inclusos eventuais blocos de �ndice). */
    DWORD   blocksFileSize; //  40:  4 bytes

    /* Tamanho do arquivo. Expresso em n�mero de bytes.
    Notar que o tamanho de um arquivo n�o � um m�ltiplo do tamanho dos blocos de dados.
    Portanto, o �ltimo bloco de dados pode n�o estar totalmente utilizado.
    Assim, a detec��o do t�rmino do arquivo depender� da observa��o desse campo do registro. */
    DWORD   bytesFileSize;  //  44:  4 bytes

    /* Dois ponteiros diretos, para blocos de dados do arquivo */
    DWORD   dataPtr[2];     //  48:  8 bytes

    /* Ponteiro de indire��o simples,
    que aponta para um bloco de �ndices onde est�o ponteiros para blocos de dados do arquivo. */
    DWORD   singleIndPtr;   //  56:  4 bytes

    /* Ponteiro de indire��o dupla,
    que aponta para um bloco de �ndices onde est�o outros ponteiros para blocos de �ndice.
    Esses �ltimos ponteiros apontam para blocos de dados do arquivo. */
    DWORD   doubleIndPtr;   //  60:  4 bytes

} __attribute__((packed));

/** Superbloco */
struct t2fs_superbloco {
    /* Identifica��o do sistema de arquivo. � formado pelas letras �T2FS�. */
    char    Id[4];          // :   4 bytes

    /* Vers�o atual desse sistema de arquivos: (valor fixo 7DE=2014; 1=1 semestre). */
    WORD    Version;        // :   2 bytes

    /* Quantidade de setores  que formam o superbloco. (fixo em 1 setor) */
    WORD    SuperBlockSize; // :   2 bytes

    /* Tamanho total da parti��o T2FS, incluindo o tamanho do superblock. (1.048.832 bytes) */
    DWORD   DiskSize;       // :   4 bytes

    /* Quantidade total de blocos de dados na parti��o T2FS (1024 blocos). */
    DWORD   NofBlocks;      // :   4 bytes

    /* Tamanho de um bloco. (1024 bytes) */
    DWORD   BlockSize;      // :   4 bytes

    /* N�o usados */
    char    Reserved[108];  // : 108 bytes

    /* Registro que descreve o arquivo que mant�m o bitmap de blocos livres e ocupados */
    struct t2fs_record BitMapReg;  // :  64 bytes

    /* Registro que descreve o arquivo que mant�m as entradas do diret�rio raiz */
    struct t2fs_record RootDirReg; // :  64 bytes

} __attribute__((packed));

/** Retorna a identifica��o dos implementadores do T2FS. */
char *t2fs_identify (void);

/** Fun��o usada para criar um novo arquivo no disco. */
t2fs_file t2fs_create (char *nome);

/** Fun��o usada para remover (apagar) um arquivo do disco. */
int t2fs_delete (char *nome);

/** Fun��o que abre um arquivo existente no disco. */
t2fs_file t2fs_open (char *nome);

/** Fun��o usada para fechar um arquivo. */
int t2fs_close (t2fs_file handle);

/** Fun��o usada para realizar a leitura em um arquivo. */
int t2fs_read (t2fs_file handle, char *buffer, int size);

/** Fun��o usada para realizar a escrita em um arquivo. */
int t2fs_write (t2fs_file handle, char *buffer, int size);

/** Altera o contador de posi��o (current pointer) do arquivo. */
int t2fs_seek (t2fs_file handle, unsigned int offset);


#endif
