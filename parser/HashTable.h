#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "struct.h"


struct TableEntry* addTableEntry(struct TableEntry** table, char* name); 
struct TableEntry* findTableEntry(struct TableEntry** table, char* name); 
void replaceTableEntry(struct TableEntry** table, struct TableEntry* entry);
void delTableEntry(struct TableEntry** table, char* name); 
void delTable(struct TableEntry** table); 
int getTableSize(struct TableEntry** table);
void addLabel(struct Label** head, char* lname);
void delLabel(struct Label** head, char* lname);
int searchLabel(struct Label* head, char* lname);
int countLabels(struct Label* l);
void addEntryLabel(struct TableEntry** table, char* entry_name, char* label);
int labelcmp(struct Label *a, struct Label *b);
int entrycmp(struct TableEntry *a, struct TableEntry *b);

/**
  struct my_struct *current_user, *tmp;
  HASH_ITER(hh, users, current_user, tmp) {...}

  struct Element* head, elt, tmp;
	LL_FOREACH_SAFE(head,elt,tmp) {…}
*/

#endif