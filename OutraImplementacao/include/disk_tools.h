#include "t2fs.h"

/* Finds a free bloc, reserve it, and returns its number 
 * return -1 if disk is full
 */
int reserve_new_bloc();

/*
 * find new free pointer, update it, and make sure
 * next free pointer is invalid. 
 * Will read and write indice blocks in the disk if needed,
 * but won't write the modified record argument in the disk
 * 
 * Returns 0 if everything alright
 * Returns -1 if repertory full
 */
int update_next_ptr(struct t2fs_record *record, DWORD addr_bloc);
