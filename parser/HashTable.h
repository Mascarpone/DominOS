/**
 * This is an API used to manage file-tag database
 * HashTables used here were based on uthash library see https://troydhanson.github.io/uthash/ for more informations
 */

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "struct.h"

/**
 * Entry: a struct with a field for the instance name
 * /
 
 /** 
 * Create a new entry of TableEntry and add it to the table, 
 * if an entry of the same name already exists return its adress.
 * @param  table HashTable, must be a double pointer instance of TableEntry
 * @param  name  name of the new instance 
 * @return       pointer to the new entry
 */
struct TableEntry* addTableEntry(struct TableEntry** table, char* name); 

/**
 * search the table looking for an entry with the same name
 * @param  table Hash table to look in
 * @param  name  name of entry
 * @return       pointer to entry if it exists, null if not.
 */
struct TableEntry* findTableEntry(struct TableEntry** table, char* name);

/**
 * replace an entry with an other one with the same name but diffrent data, delete the old one
 * @param table table to look in
 * @param entry new entry
 */
void replaceTableEntry(struct TableEntry** table, struct TableEntry* entry);

/**
 * delete entry from table
 * @param table table to edit
 * @param name  entry name
 */
void delTableEntry(struct TableEntry** table, char* name); 

/**
 * delete the whole table reccursivly 
 * @param table table to delete
 */
void delTable(struct TableEntry** table);

/**
 * count how many entrys stored in the table
 * @param  table table to mesure
 * @return       number of entrys
 */
int getTableSize(struct TableEntry** table);

/**
 * Element: a struct with a field for the instance name
 * /

/**
 * creat a new element and add it to the linked list
 * @param head  Linked list head
 * @param lname name of element to add
 */
void addLabel(struct Label** head, char* lname);

/**
 * delete an element from linked list
 * @param head  Linked list head
 * @param lname element name
 */
void delLabel(struct Label** head, char* lname);

/**
 * Search a linked list looking for element with given name
 * @param  head  Linked list head
 * @param  lname element name
 * @return       element adressif found, null if not
 */
int searchLabel(struct Label* head, char* lname);

/**
 * count how many elements in the list
 * @param  head Linked list head
 * @return      number of stored elements
 */
int countLabels(struct Label* head);

/**
 * add automaticly an entry (if not found) and add a element to its linked list
 * @param table      table to edit
 * @param entry_name entry to edit
 * @param label      element to add
 */
void addEntryLabel(struct TableEntry** table, char* entry_name, char* label);

/**
 * compare two labels, based on a strcmp
 * @param  a 
 * @param  b 
 * @return   
 */
int labelcmp(struct Label *a, struct Label *b);

/**
 * compare two entry names, based on a strcmp
 * @param  a 
 * @param  b 
 * @return   
 */
int entrycmp(struct TableEntry *a, struct TableEntry *b);

/**
  struct my_struct *current_user, *tmp;
  HASH_ITER(hh, users, current_user, tmp) {...}

  struct Element* head, elt, tmp;
	LL_FOREACH_SAFE(head,elt,tmp) {…}
*/

#endif