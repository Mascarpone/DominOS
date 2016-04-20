#include "parser.h"

void parse(char* fName, struct TableEntry** file_tags, struct TableEntry** tag_files) {
  char line[256], last_file[256], string[256];
  FILE *file = fopen(fName, "r");
  struct Label* tag_all = NULL;
  struct Label* tmp = NULL; 
  struct Label* current1 = NULL; 
  struct Label* current2 = NULL;
  struct TableEntry* entry = NULL;
  int pos;

  if (file == 0) {
    perror("Cannot open input file");
    exit(-1);
  }
  // Scan All-files tags
  while (fgets(line, 20, file) != NULL) {
    if (line[0]=='\n' || line[0]=='\0' || line[0]=='#') {
      continue;
    }
    if(line[0]=='['){
      fseek(file, pos, SEEK_SET);
      break;
    }
    else {
      sscanf(line, "%s", string);
      addLabel(&tag_all, string);
      addTableEntry(tag_files, string);

    }
    pos = ftell(file);
  }

  while (fgets(line, 20, file) != NULL) {
    if (line[0]=='\n' || line[0]=='\0' || line[0]=='#') {
      continue;
    }
    if(line[0]=='['){
      sscanf(line, "[%99[^]]]", last_file);
      entry = addTableEntry(file_tags, last_file);

      // Add All-files tags
      if (tag_all != NULL){
        LL_FOREACH_SAFE(tag_all,current1,tmp) {
          LL_SEARCH(entry->head,current2,current1,labelcmp);
          if (current2 == NULL){
            addEntryLabel(file_tags, last_file, current1->name);
            addEntryLabel(tag_files, current1->name, last_file);
          }
        }
      }
    }
    else {
      sscanf(line, "%s", string);
      addTableEntry(tag_files, string);
      addEntryLabel(file_tags, last_file, string);
      addEntryLabel(tag_files, string, last_file);
    }
  }


  fclose(file);
}

// void main(){
//   char* fName = ".tag";
//   struct TableEntry* file_tags = NULL;
//   struct TableEntry* tag_files = NULL;
//   parse(fName, &file_tags, &tag_files);
// 
//   struct TableEntry *current, *tmp;
//   struct Label *lbl;
//   printf("file_tags content:\n");
//   HASH_ITER(hh, file_tags, current, tmp){
//     printf("[%s]\n",current->name);
//     LL_FOREACH(current->head,lbl) printf("%s\n", lbl->name);
//   }
//   printf("\ntag_files content:\n");
//   HASH_ITER(hh, tag_files, current, tmp){
//     printf("[%s]\n",current->name);
//     LL_FOREACH(current->head,lbl) printf("%s\n", lbl->name);
//   }
//   
//   delTable(&file_tags);
//   delTable(&tag_files);
// }
