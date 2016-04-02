#include <string.h>  
#include <stdlib.h>  
#include <stdio.h>   
#include "uthash.h"
#include "utlist.h"
#include "HashTable.h"



void addTableEntry(struct HashEntry** table, char* string) {
	struct HashEntry* entry;
	HASH_FIND_STR(*table, string, entry); /* string already in the hash? */
	if (entry == NULL){
		entry = malloc(sizeof(struct HashEntry));
		entry->list = NULL;
		strcpy(entry->string, string);	
		HASH_ADD_STR(*table, string, entry);
	}    
}

struct HashEntry* findTableEntry(struct HashEntry** table, char* string) {
  struct HashEntry* entry;
  HASH_FIND_STR(*table, string, entry);
  return entry;
}

void replaceTableEntry(struct HashEntry **table, struct HashEntry *entry){
	if(entry == NULL)
		exit(EXIT_FAILURE);

	struct HashEntry *tmp;
  char* string = entry->string;
  HASH_FIND_STR(*table, string, tmp);
  HASH_DEL(*table, tmp); 
  free(tmp);
  HASH_ADD_STR(*table, string, entry);
}

void delTableEntry(struct HashEntry** table, char* string) {
  struct HashEntry* entry;
  HASH_FIND_STR(*table, string, entry);
  HASH_DEL( *table, entry);  
  free(entry);            
}

void delTable(struct HashEntry** table) {
  struct HashEntry *current, *tmp;
  HASH_ITER(hh, *table, current, tmp) {
    HASH_DEL( *table, current);  
    free(current);            
  }
}

int getTableSize(struct HashEntry** table){
	return HASH_COUNT(*table);
}



void delListElement(struct LinkedList* ll, char* string){
  struct Element* head = ll->head;
  struct Element* del = malloc(sizeof(struct Element));
  strcpy(add->string, string);
  ll->count--;
  LL_DELETE(head,del);
}

void addListElement(struct LinkedList* ll, char* string){
  struct Element* head = ll->head;
  struct Element* add = malloc(sizeof(struct Element));
  strcpy(add->string, string);
  ll->count++;
  LL_PREPEND(head,add);
}
