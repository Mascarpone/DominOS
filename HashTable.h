#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "uthash.h"

struct Element {
    char string[256];
    struct Element *next;
};

struct LinkedList {
	struct Element* head;
	int count = 0;
};

struct HashEntry {
	char string[256]; /* we'll use this field as the key */
	struct LinkedList* list;
  UT_hash_handle hh; /* makes this structure hashable */
};

void addTableEntry(struct HashEntry** table, char* name); 
struct HashEntry* findTableEntry(struct HashEntry** table, char* name); 
void replaceTableEntry(struct HashEntry** table, struct HashEntry* entry);
void delTableEntry(struct HashEntry** table, char* name); 
void delTable(struct HashEntry** table); 
int getTableSize(struct HashEntry** table);

#endif