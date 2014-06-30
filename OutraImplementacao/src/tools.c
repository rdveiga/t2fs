#include "t2fs.h"
#include "apidisk.h"

/*
 * NO INTERACTION WITH DISK FUNCTIONS HERE
 */


/*
 * Local implementation of strncpy, since no library
 * can be used
 */
char *tools_strncpy(char *dest, const char *src, int n)
{
    char *d = dest;

    while (*src != '\0') {
        if (!n--)
            return dest;
        *d++ = *src++;
    }
    while (n--)
        *d++ = '\0';
    return dest;
} 

/* Returns 0 if the name is valid (up to 39 char, 
 * range [0x21, 0x7A], null-terminated).
 * Returns 1 if not null-terminated.
 * Returns 2 if it contains an invalid char.
 */
int tools_valid_name (char *name) {
    int i = 0;
    char cursor = *name;

    while (cursor != '\0') {
        if (i == 40)
            return 1;

        cursor = name[i];

        if (cursor < 0x21 || cursor > 0x7A)    
            return 2;

        i++;
    }
    return 0;
}

unsigned int addr_to_sect(DWORD address) {
    return (unsigned int) (address * 4 +1);
}


/*
 * Initialize the fields of a new empty record
 */
void fill_new_record(struct t2fs_record *record, char *name) {
    //TypeVal
    record->TypeVal = TYPEVAL_REGULAR;
    //Name
    switch (tools_valid_name(name)) {
        case 0:
            tools_strncpy(record->name, name, 39);
            break;
        case 1:
            tools_strncpy(record->name, name, 38);
            (record->name)[38] = '\0';
            break;
        case 2:
            // 39 so it will fill the rest with '\0'
            tools_strncpy(record->name, "new_file", 39);
            break;
    }
    //blocskFileSize
    record->blocksFileSize = (DWORD) 0;
    //bytesFileSize
    record->bytesFileSize = (DWORD) 0;
    //dataPtr
    record->dataPtr[0] = INVALID_PTR;
    record->dataPtr[1] = INVALID_PTR;
    //singleIndPtr
    record->singleIndPtr = INVALID_PTR;
    //doubleIndPtr
    record->doubleIndPtr = INVALID_PTR;
}
