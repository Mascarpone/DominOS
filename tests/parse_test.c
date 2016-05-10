#include "../parser/parser.h"

// Take tag file as argument
int main(int argc, char** argv){
 
  if (argc < 2) {
    printf("usage: %s <.tags file path>\n", argv[0]);
    exit(1);
  }
 
  struct TableEntry* file_tags = NULL;
  struct TableEntry* tag_files = NULL;
  parse(argv[1], &file_tags, &tag_files);

  struct TableEntry *current, *tmp;
  struct Label *lbl;
  printf("file_tags content:\n");
  HASH_ITER(hh, file_tags, current, tmp){
    printf("[%s]\n",current->name);
    LL_FOREACH(current->head,lbl) printf("%s\n", lbl->name);
  }
  printf("\ntag_files content:\n");
  HASH_ITER(hh, tag_files, current, tmp){
    printf("[%s]\n",current->name);
    LL_FOREACH(current->head,lbl) printf("%s\n", lbl->name);
  }
  
  delTable(&file_tags);
  delTable(&tag_files);
  return 0;
}
