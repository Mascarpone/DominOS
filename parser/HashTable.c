#include <stdlib.h>  
#include <stdio.h> 
#include <string.h>
#include "HashTable.h"


struct TableEntry* addTableEntry(struct TableEntry** table, char* name) {
	struct TableEntry* entry;
	HASH_FIND_STR(*table, name, entry); /* name already in the hash? */
	if (entry == NULL){
		entry = malloc(sizeof(struct TableEntry));
		entry->head = NULL;
		strcpy(entry->name, name);
		HASH_ADD_STR(*table, name, entry);
		return entry;
	}
	return NULL;    
}

struct TableEntry* findTableEntry(struct TableEntry** table, char* name) {
  struct TableEntry* entry;
  HASH_FIND_STR(*table, name, entry);
  return entry;
}


void replaceTableEntry(struct TableEntry **table, struct TableEntry *entry){
	if(entry == NULL)
		exit(EXIT_FAILURE);
	struct TableEntry *tmp;
  char* name = entry->name;
  HASH_FIND_STR(*table, name, tmp);
  HASH_DEL(*table, tmp); 
  free(tmp);
  HASH_ADD_STR(*table, name, entry);
}



void delTableEntry(struct TableEntry** table, char* name) {
  struct TableEntry* entry;
  HASH_FIND_STR(*table, name, entry);
  HASH_DEL(*table, entry);  
  free(entry);            
}

void delTable(struct TableEntry** table) {
  struct TableEntry *current, *tmp;
	struct Label *llcurrent, *lltmp;
  HASH_ITER(hh, *table, current, tmp) {
		LL_FOREACH_SAFE(current->head,llcurrent,lltmp) {
			LL_DELETE(current->head, llcurrent);
			free(llcurrent);
		}
    HASH_DEL(*table, current);  
    free(current);            
  }
	free(*table);
}

int getTableSize(struct TableEntry** table){
	return HASH_COUNT(*table);
}

int labelcmp(struct Label *a, struct Label *b) {
  return strcmp(a->name,b->name);
}

int entrycmp(struct TableEntry *a, struct TableEntry *b) {
  return strcmp(a->name,b->name);
}

void addLabel(struct Label** head, char* lname){
  struct Label* add = malloc(sizeof(struct Label));
  strcpy(add->name, lname);
  add->next = NULL;
  struct Label* tmp;
  LL_SEARCH(*head, tmp , add, labelcmp); /*label already in the list ?*/
  if (tmp) {
		free(add);
    return;
	}
  LL_PREPEND(*head,add);
}

void delLabel(struct Label** head, char* lname){
	struct Label* todel;
  struct Label* dellike = malloc(sizeof(struct Label));
	strcpy(dellike->name, lname);
	dellike->next = NULL;
	LL_SEARCH(*head, todel, dellike, labelcmp);
  LL_DELETE(*head, todel);
	free(todel);
	free(dellike);
}

int searchLabel(struct Label* head, char* lname) {
  struct Label* add = malloc(sizeof(struct Label));
  strcpy(add->name, lname);
  add->next = NULL;
  struct Label* tmp;
  LL_SEARCH(head, tmp , add, labelcmp);
  if (tmp)
    return 1;
  else
    return 0;
}

int countLabels(struct Label* l){
  struct Label* elt;
  int count;
  LL_COUNT(l,elt,count);
  return count;
}

void addEntryLabel(struct TableEntry** table, char* entry_name, char* label){
  struct TableEntry* entry = findTableEntry(table, entry_name);
  if (entry) {
    addLabel(&(entry->head), label);
	}
  else {
    perror("Cannot find entry\n");
    exit(-1);
  }
}