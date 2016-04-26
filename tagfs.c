/*
 * Copyright (c) 2016 dominOS. All rights reserved.
 * File System by gcombes001, hdodelin, ohayak and flevern.
 */

#define _GNU_SOURCE
#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>

/*******************
 * Logs
 */

#define LOGFILE "tagfs.log"
FILE *mylog;
#define LOG(args...) do { fprintf(mylog, args); fflush(mylog); } while (0)

/*******************
 * Parser 
 */

#include "parser/HashTable.h"
#include "parser/parser.h"

/*******************
 * Globals
 */
 
static DIR *dir;
static char *dirpath;
struct TableEntry * tag_files = NULL;
struct TableEntry * file_tags = NULL;

/*******************
 * File operations
 */

/* get the path of the file in the VFS */
char * tag_realpath(const char *path) {
   char *realpath;
   char *lastslash = strrchr(path, '/');
   asprintf(&realpath, "%s/%s", dirpath, (lastslash == NULL) ? "" : lastslash + 1);
   return realpath;
}

/* get the tags in the path into the path_tags table */
int tag_fillpathtags(char ** path_tags, const char * path) {
  int i = 0;
  char * pathcpy = malloc(sizeof(char)*strlen(path));
  strcpy(pathcpy, path);
  
  char * tok = strtok(pathcpy, "/");
  while(tok != NULL) {
    path_tags[i] = malloc(sizeof(char)*strlen(tok));
    strcpy(path_tags[i], tok);
    tok = strtok(NULL, "/");
    i++;
  }
  
  free(pathcpy);
  return i;
}

/* get attributes */
static int tag_getattr(const char *path, struct stat *stbuf) {
  char *realpath = tag_realpath(path);
  int res;

  LOG("getattr '%s'\n", path);
  
  res = stat(realpath, stbuf);
  if (res < 0 && errno == ENOENT)
    res = stat(dirpath, stbuf);

  free(realpath);
  return res;
}

/* list files within directory */
static int tag_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    
  LOG("readdir '%s'\n", path);
  
  int hastags;
  int i = 0;
  struct TableEntry *current_file, *tmp;
  struct Label *current_tag;
  struct TableEntry *tag_folders = NULL;
  char ** path_tags = malloc(sizeof(char*)*(getTableSize(&tag_files) + 1));
  int s = tag_fillpathtags(path_tags, path); // get the tags in the path
  
  HASH_ITER(hh, file_tags, current_file, tmp) {
    // case of the root folder
    if (!strcmp(path, "/")) { 
      filler(buf, current_file->name, NULL, 0); 
    }
    // default case
    else {
      // test if the file has all the tags in the path
      hastags = 1;
      current_tag = NULL;
      for (int j = 0; j < s; j++) {
        hastags = 0;
        LL_FOREACH(current_file->head, current_tag) {
          if (!strcmp(current_tag->name, path_tags[j])) {
            hastags = 1;
            break;
          }
        }
        if (!hastags) break;
        current_tag = NULL;
      }
      if (!hastags) continue;
      
      // display the file
      filler(buf, current_file->name, NULL, 0);
    }
      
    // get its tags to create folders
    current_tag = NULL;
    LL_FOREACH(current_file->head, current_tag) {
      // add only the tags that are not already in the path
      for (int j = 0; j < s; j++) {
        if (!strcmp(path_tags[j], current_tag->name)) goto next;
      }
      addTableEntry(&tag_folders, current_tag->name);
      next:
      continue;
    }
  }

  // display the folders corresponding to the tags 
  HASH_ITER(hh, tag_folders, current_file, tmp) {
    filler(buf, current_file->name, NULL, 0);
  }
  
  delTable(&tag_folders);
  for(int j = 0; j < s; j++) free(path_tags[j]);
  free(path_tags);
  return 0;
}

/* read the content of the file */
int tag_read(const char *path, char *buffer, size_t len, off_t off, struct fuse_file_info *fi) {
  char *realpath = tag_realpath(path);
  char *command;
  FILE *fd;
  char *cmdoutput;
  int res;

  LOG("read '%s' for %ld bytes starting at offset %ld\n", path, len, off);

  asprintf(&command, "cat %s", realpath);
  LOG("read using command %s\n", command);

  /* open the output of the command */
  fd = popen(command, "r");
  if (!fd) {
    res = -errno;
    goto out;
  }

  /* read up to len+off bytes from the command output */
  cmdoutput = malloc(len+off);
  if (!cmdoutput) {
    res = -ENOMEM;
    goto out_with_fd;
  }
  res = fread(cmdoutput, 1, len+off, fd);
  LOG("read got %d bytes out of %ld requested\n", res, len+off);
  if (res > off) {
    /* we read more than off, ignore the off first bytes and copy the remaining ones */
    memcpy(buffer, cmdoutput+off, res-off);
    res -= off;
  } else {
    /* we failed to read enough */
    res = 0;
  }

  free(cmdoutput);
  free(command);

 out_with_fd:
  pclose(fd);
 out:
  if (res < 0)
    LOG("read returning %s\n", strerror(-res));
  else
    LOG("read returning success (read %d)\n", res);
  free(realpath);
  return res;
}

/* add a new tag to file 
 * example: ln a.jpg baz/a.jpg 
 */
int tag_link(const char* from, const char* to) {
  return 0;
}

/* change a tag into another 
 * example: mv foo/a.jpg baz/a.jpg 
 */
int tag_rename(const char* from, const char* to) {
  return 0;
}

/* remove a tag from a file 
 * example: rm montag/a.jpg 
 */
int tag_unlink(const char* path) {
  return 0;
}

/* create a new tag not yet bound to a file 
 * example: mkdir montag/
 */
int tag_mkdir(const char* path, mode_t mode) {
  return 0;
}

/* remove a non used tag 
 * example: rmdir montag
 */
int tag_rmdir(const char* path) {
  return 0;
}

static struct fuse_operations tag_oper = {
  .getattr = tag_getattr,
  .readdir = tag_readdir,
  .read = tag_read,
  .link = tag_link,
  .rename = tag_rename,
  .unlink = tag_unlink,
  .mkdir = tag_mkdir,
  .rmdir = tag_rmdir
};

/*******************
 * Main
 */

int main(int argc, char ** argv) {
  int err;

  if (argc < 2) {
    fprintf(stderr, "missing destination directory\n");
    exit(EXIT_FAILURE);
  }

  dirpath = realpath(argv[1], NULL);
  dir = opendir(dirpath);
  if (!dir) {
    fprintf(stderr, "couldn't open directory %s\n", dirpath);
    exit(EXIT_FAILURE);
  }
  argv++;
  argc--;

  mylog = fopen(LOGFILE, "a");
  LOG("\n");
  
  // parse the .tags file to init global hash tables
  char *tagfilepath = malloc(sizeof(char)*(strlen(dirpath)+7));
  strcpy(tagfilepath, dirpath);
  strcat(tagfilepath, "/.tags"); 
  parse(tagfilepath, &file_tags, &tag_files);
  free(tagfilepath);
  
  // complete the file_tags hashtable with the files that don't have a tag
  struct dirent *dirent;
  rewinddir(dir);
  while ((dirent = readdir(dir)) != NULL) {
    if(!findTableEntry(&file_tags, dirent->d_name)) {
      addTableEntry(&file_tags, dirent->d_name);
    }
  }
  
  LOG("starting tagfs in %s\n", dirpath);
  err = fuse_main(argc, argv, &tag_oper, NULL);
  LOG("stopped tagfs with return code %d\n", err);

  closedir(dir);
  free(dirpath);

  LOG("\n");
  delTable(&tag_files);
  delTable(&file_tags);
  return err;
}
