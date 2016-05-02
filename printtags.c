#include "tagioctl.h"

int main(int argc, char ** argv) {
  
  if (argc != 2) {
    printf("usage: %s <path_to_file>\n", argv[0]);
    return EXIT_FAILURE;
  }
  
  /* send a specific request to the fuse deamon filesystem */
  int fd = open(argv[1], O_RDONLY);
  if (fd == -1) { perror("open()"); return EXIT_FAILURE; }
  
  struct tagslist tags;
  if (ioctl(fd, IOC_GET_TAGS, &tags) == -1) { perror("ioctl()"); return EXIT_FAILURE; } 
  
  // print all the tags in the returned structure
  for (int i = 0; i < tags.tags_nb; i++) {
    printf("%s\n", tags.tags[i]);
  }
  
  return EXIT_SUCCESS;
}