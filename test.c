#include "HashTable.h"
#include <string.h>
#include <string.h>  
#include <stdlib.h>  
#include <stdio.h>   

int main(int argc, char *argv[]) {
    char **n, *names[] = { "joe", "bob", "betty", NULL };
    char buff[] = "betty";
    struct HashEntry **table = malloc(sizeof(struct HashEntry));
    struct HashEntry *tmp, *entry=NULL;
    int i=0;

    for (n = names; *n != NULL; n++) {
        addTableEntry(table, *n);
    }
    
    entry = findTableEntry(table,buff);
    if (entry != NULL) 
        printf("betty's id is here\n");
    delTableEntry(table,buff);
    entry = findTableEntry(table,buff);
    if (entry == NULL) 
        printf("betty's id is not here\n");
    /* free the hash table contents */
    delTable(table);
    
    return 0;
}