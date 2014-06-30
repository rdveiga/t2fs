#include "t2fs.h"
#include "apidisk.h"
#include "tools.h"
#include "read_only_tools.h"

DWORD reserve_new_bloc() {
/*
 * Read bitmap file from disk 
 * find free bit
 * reserve bit
 * write bitmap sector to disk
 * return bit position*1024: address
 */
    int i;

    unsigned int first_sector; //bitmap first sector
    BYTE sector[BYTES_PER_SECTOR];  //to parse bloc sector
    unsigned int sector_offset, //sector number in bitmap bloc
        pos;      //bit position
    BYTE mask;

    first_sector = addr_to_sect(bitmap_address());

/*** Find free bit ***/
    //loop on SECTORS from bloc
    for (sector_offset = 0; sector_offset < 4; sector_offset++) {
        read_sector(first_sector + sector_offset, (char*) sector);
        //loop on BYTES from sector
        for (i = 0; i < BYTES_PER_SECTOR; i++) {
            //loop on BITS from byte
            for (pos = 0; pos < 8; pos++) {
                mask = 1<<pos;
                if ((sector[i] & mask) == 0) {
                    //just found a 0-bit
                    //goto is ugly, but best way to break
                    //nested loops and keep variable values
                    goto end_loop_sector;
                }
            }
        }
        if (sector_offset == 3)
            //no more free bits
            return -1;
    }
end_loop_sector:

/*** Reserve free bit ***/
    //if no more free bit, function exited before
    sector[i] = sector[i] | mask;    

/*** Write modified sector do disk ***/
    write_sector(first_sector + sector_offset, (char*) sector);

/*** Return bit position as an address ***/
    return (DWORD) (sector_offset * BYTES_PER_SECTOR 
                        + i * 8 //bytes offset in sector
                        + pos); //bits offset in byte
}

/*
 * INTERNAL USE ONLY. DO NOT INCLUDE IN HEADER, STUPID!
 *
 * return 0 if everything went fine, or -1 if all pointers are already in use
 * nextIndPtr set to invalid if updated ptr is the last of singleInd
 *
int update_next_ptr_in_singleInd_block(DWORD *singleIndPtr, DWORD addr_bloc, DWORD *nextIndPtr) {
    int i;

    //1024 bytes per block
    //4 bytes per address -> 256 addresses per block
    DWORD indice_block1[256]; 
    
    //if singleInd block doesn't exist yet. create it, update it and write it.
    if (*singleIndPtr == INVALID_PTR) {
        *singleIndPtr = reserve_new_bloc();
        indice_block1[0] = addr_bloc;
        indice_block1[1] = INVALID_PTR;
        write_sector(addr_to_sect(*singleIndPtr), (char*) indice_block1);
        return 0;
    }

    //get singleInd block
    for (i = 0; i < 4; i++) {
        read_sector(addr_to_sect(*singleIndPtr) + i, (char*) (indice_block1 + 64*i));
    }

    //check all ptr of singleInd BUT THE LAST
    for (i = 0; i < 255; i++) {
        if (indice_block1[i] == INVALID_PTR) {
            indice_block1[i] = addr_bloc;
            indice_block1[i+1] = INVALID_PTR;
            //NB: floor(i/64) = modified sector number in block1
            write_sector(addr_to_sect(*singleIndPtr) + (int) (i/64), 
                    (char*) (indice_block1 + 64 * ((int) (i/64))));
            return 0;
        }
    }

    //check last ptr of singleInd
    if (indice_block1[255] == INVALID_PTR) {
        indice_block1[255] = addr_bloc;
        write_sector(addr_to_sect(*singleIndPtr) + 3, 
                (char*) (indice_block1 + 192)); //64 * 3 = 192
        *nextIndPtr = INVALID_PTR;
        return 0;
    }

    return -1; //all ptrs are used
}
*/

/*
 * INTERNAL USE ONLY. DO NOT INCLUDE IN HEADER, STUPID!
 *
 * return 0 if everything went fine, or -1 if all pointers are already in use
 *
int update_next_ptr_in_doubleInd_block(DWORD *doubleIndPtr, DWORD addr_bloc) {

    int i;

    //1024 bytes per block
    //4 bytes per address -> 256 addresses per block
    DWORD indice_block2[256];
    DWORD indice_block1[256];
    DWORD fake_address; 
    //fake_address is used to avoid a segfault if the next free ptr is the last
    //of the lasts, so there is no one to set as invalid.


    //check doubleIndPtr: doesn't exist yet
    if (*doubleIndPtr == INVALID_PTR) {
        * reserve double/single Indice blocks 
         * update double/ single indice blocks
         * write double/single indice block
         *

        *doubleIndPtr = reserve_new_bloc();
        indice_block2[0] = reserve_new_bloc(); //address of single ind to create
        indice_block2[1] = INVALID_PTR;

        indice_block1[0] = addr_bloc;
        indice_block1[1] = INVALID_PTR;

        write_sector(addr_to_sect(*doubleIndPtr), (char*) (indice_block2));
        write_sector(addr_to_sect(indice_block2[0]), (char*) (indice_block1));
        return 0;
    }

    //doubleIndPtr exists. 
    //get doubleInd block
    for (i = 0; i < 4; i++) {
        read_sector(addr_to_sect(*doubleIndPtr) + i, (char*) (indice_block2 + 64*i));
    }

    // NB: this loop does not treat the last singleInd block
    for (i = 0; i < 255; i++) {
        if (update_next_ptr_in_singleInd_block
                (&(indice_block2[i]), addr_bloc, &(indice_block2[i+1])) == 0) {
            // found a free pointer and updated it
            // now write back the doubleInd block and return
            //NB: floor(i/64) = modified sector number in block2
            write_sector(addr_to_sect(*doubleIndPtr) + (int) (i/64), 
                    (char*) (indice_block2 + 64 * ((int) (i/64))));
            return 0;
        }
    }

    //now the last singleInd block
    if (update_next_ptr_in_singleInd_block
            (&(indice_block2[255]), addr_bloc, &fake_address) == 0) {
        write_sector(addr_to_sect(*doubleIndPtr) + 3, 
                (char*) (indice_block2 + 192)); //64 * 3 = 192
        return 0;

    }

    return -1; //all pointers are already used
}

*
 * find new free pointer, update it, and make sure
 * next free pointer is invalid. 
 * Will read and write indice blocks in the disk if needed,
 * but won't write the modified record argument in the disk
 * 
 * Returns 0 if everything alright
 * Returns -1 if repertory full
 */
int update_next_ptr(struct t2fs_record *record, DWORD addr_bloc) {
    return 0;
}
    /*
// checking dataPtr  
    if (record->dataPtr[0] == INVALID_PTR) {
        record->dataPtr[0] = addr_bloc;
        record->dataPtr[1] = INVALID_PTR;
        return 0;
    }
    if (record->dataPtr[1] == INVALID_PTR) {
        record->dataPtr[1] = addr_bloc;
        record->singleIndPtr = INVALID_PTR;
        return 0;
    }

// checking singleIndPtr
    if (update_next_ptr_in_singleInd_block
            (&(record->singleIndPtr), addr_bloc, &(record->doubleIndPtr)) == 0)
        return 0;

// checking doubleIndPtr
    if (update_next_ptr_in_doubleInd_block(&(record->doubleIndPtr), addr_bloc) == 0)
        return 0;

    return -1; //repertory is full, too bad!
}
*/
