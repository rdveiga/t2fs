#include "t2fs.h"

char *tools_strncpy(char *dest, const char *src, int n);
int tools_valid_name(char *name);

/* return the first sector of the corresponding address */
    unsigned int addr_to_sect(DWORD address);

/*
 * Initialize the fields of a new empty record
 */
void fill_new_record(struct t2fs_record *record, char *name);
