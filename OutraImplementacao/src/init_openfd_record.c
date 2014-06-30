/*
 * TO BE USED ONLY ONCE, TO INIALIZE THE DISK
 *
 * Create a record starting on the 64th Byte of sector 0.
 * This record describes the file containing the table of
 * open file descriptors.
 * A file descriptor contains a pointer to the corresponding 
 * record, and the current pointer.
 * A file handler is the position of the file descriptor in 
 * this table.
 * After the last element of the table, there is 0xFFFFFFFF
 */

#include "t2fs.h"
#include "disk_tools.h"
#include "apidisk.h"
#include "tools.h"

int main() {
    
    int i;
    struct file_descriptor fd[32];

    // set the record in sector 0
    struct t2fs_superbloco superblock;
    read_sector(0, (char*) &superblock);

    superblock.FileDescrReg.TypeVal = 1;
    tools_strncpy(superblock.FileDescrReg.name, "File Decriptor Table", 39);
    superblock.FileDescrReg.blocksFileSize = 1;
    superblock.FileDescrReg.bytesFileSize = 1024;
    superblock.FileDescrReg.dataPtr[0] = reserve_new_bloc();
    superblock.FileDescrReg.dataPtr[1] = INVALID_PTR;
    superblock.FileDescrReg.singleIndPtr = INVALID_PTR;
    superblock.FileDescrReg.doubleIndPtr = INVALID_PTR;

    write_sector(0, (char*) &superblock);

    // initialization of table
    // we only use the first sector of the block
    // since the system is not supposed to be able to 
    // manage more than 20 simultaneous opened files
    for (i = 0; i < 32; i++) {
        fd[i] = (struct file_descriptor) {.record_address = INVALID_PTR, .current_pointer = 0};
    }

    write_sector(addr_to_sect(superblock.FileDescrReg.dataPtr[0]), (char*) &fd);

    return 0;
}
