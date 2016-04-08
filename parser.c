#include "HashTable.h"

void parse(char* fName, struct TableEntry** file_tags, struct TableEntry** tag_files) {
  char* line, string, last_file;
  FILE *file = fopen(fName, "r");

  if (file == 0) {
    perror("Canot open input file\n");
    exit(-1);

  }else {
    while (fgets(line, 20, file) != EOF ){
      if(line[0]=='['){
        sscanf(line, "[%s]", string);
        addTableEntry(file_tags, string);
        strcpy(last_file, string);
      }
      else if (line[0]=='\n' || line[0]=='\0' )
      else{
        sscanf(line, "%s", string);
        addTableEntry(tag_files, string);
        addEntryLabel(file_tags, last_file, string);
        addEntryLabel(tag_files, string, last_file);
      }
    }

    fclose(file);
  }
}

void main(){
  char* fName = ".tag";
  struct TableEntry** file_tags = malloc(sizeof(struct TableEntry));
  struct TableEntry** tag_files = malloc(sizeof(struct TableEntry));
  parse(fName, file_tags, tag_files);

  struct TableEntry* current;
  struct Label* lbl, tmp;
  HASH_ITER(hh, file_tags, current, tmp){
    printf("[%s]",current->name);
    LL_FOREACH_SAFE(current->head,lbl,tmp) {
      
    }
  }
  
}