#include "tools.h"
#include "t2fs.h"
#include "apidisk.h"
#include "disk_tools.h"
#include "read_only_tools.h"

/* Retorna a identificação dos desenvolvedores do T2FS */
char *t2fs_identify() {
    return "";
}

/* Cria um novo arquivo no disco */
// the handle returned is the address of the archive record
t2fs_file t2fs_create (char *name) {
//TODO: reprendre tout ça...

    struct t2fs_superbloco superbloco;
    t2fs_file addr_bloc;

    /* create new record */
    struct t2fs_record record;
    fill_new_record(&record, name);

    /* Find a free block, reserve it and write record in it */
    addr_bloc = reserve_new_bloc();
    if (addr_bloc < 0)
        return -1; //disk full

    write_sector(addr_to_sect(addr_bloc), (char*) &record);

    /* Update the pointer list of the repertory containing 
     * the new file/repertory */
    // TODO: insérer en concordance avec adresse
    if (update_next_ptr(&(superbloco.RootDirReg), addr_bloc) < 0)
        return -2; //repertory full
    
    /* Write back the superblock in case it has been modified */
    write_sector(0, (char*) &superbloco);
    
    return addr_bloc;
}

/** Função usada para remover (apagar) um arquivo do disco. */
int t2fs_delete (char *nome) {
    return 0;
}

/** Função que abre um arquivo existente no disco. */
// Returns -1 if incorrect name
// returns -2 if too many files are opened
t2fs_file t2fs_open (char *nome) {

    int i;
    struct t2fs_record opened_file_record; //record describing the opened archive
    struct file_descriptor fd_table[32];

    //so as to initialize the future entry in the fd table, field
    //address of record describing current archive
    DWORD record_address = get_record_address_from_name(nome);

    if (record_address == INVALID_PTR) 
        return -1;
    else
        opened_file_record = get_record_from_address(record_address);
        //so as to get the first pointer of the record and then initialize current_ptr

    //now we will get the file descriptor table, update it and write it back
    read_sector(addr_to_sect(get_fd_table_address()), (char*) &fd_table);

    for (i = 0; i < 32; i++) {
        if (fd_table[i].record_address == INVALID_PTR) {
            // we found a free place in the table
            fd_table[i].record_address = record_address;
            fd_table[i].current_pointer = opened_file_record.dataPtr[0];
            write_sector(addr_to_sect(get_fd_table_address()), (char*) &fd_table);
            return i;
        }
    }

    return -2; //no free place found, too many files are already opened.
}

/** Função usada para fechar um arquivo. */
int t2fs_close (t2fs_file handle) {

    struct file_descriptor fd_table[32];
    
    read_sector(addr_to_sect(get_fd_table_address()), (char*) &fd_table);

    fd_table[handle].record_address = INVALID_PTR; 
    fd_table[handle].current_pointer = INVALID_PTR; 
    
    write_sector(addr_to_sect(get_fd_table_address()), (char*) &fd_table);
    
    return 0;
}

/** Função usada para realizar a leitura em um arquivo. */
/* Read 'size' bytes of file identified by "handle", and 
 * write then in the buffer.
 * Returns number of bytes read. 
 */
//PREMIER INCREMENT : dir / doit fonctionner, SIZE = 256
int t2fs_read (t2fs_file handle, char *buffer, int size) {
    // Get current pointer and record_address from fd_table
    struct file_descriptor fd[32];
    read_sector(addr_to_sect(get_fd_table_address()), (char*) &fd);
    //DWORD current_pointer = fd[handle].current_pointer;
    DWORD record_address = fd[handle].record_address;

    //Get the record describing the file to read
    struct t2fs_record record = get_record_from_address(record_address);

    //read the bytes
    //int remaining_bytes = size;

    //premier incrément: lecture du premier secteur désigné par dataPtr[0],
    read_sector(addr_to_sect(record.dataPtr[0]), buffer);

    //TODO continuer ici
    //while (remaining_bytes > 0) {

    // lire max(taille_restante, bloc size)
    // si on a atteind la fin du bloc
    // prendre pointer suivant
    // répéter
    
    //}
    return 0;
}

/** Função usada para realizar a escrita em um arquivo. */
int t2fs_write (t2fs_file handle, char *buffer, int size) {
    return 0;
}

/** Altera o contador de posição (current pointer) do arquivo. */
int t2fs_seek (t2fs_file handle, unsigned int offset) {
    return 0;
}
