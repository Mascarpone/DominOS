#ifndef STRUCT_H
#define STRUCT_H

#include "uthash.h"
#include "utlist.h"

struct Label {
    char name[256];
    struct ListEntry *next;
};

struct TableEntry {
	char name[256]; /* we'll use this field as the key */
	struct Label* head;
  UT_hash_handle hh; /* makes this structure hashable */
};

#endif