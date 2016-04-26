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
  int res = 0;
  char ** path_tags = malloc(sizeof(char*)*(getTableSize(&tag_files) + 1));
  int s = tag_fillpathtags(path_tags, path);

  LOG("getattr '%s'", path);
  
  if (!s) { // the path concerns the root folder
    res = stat(dirpath, stbuf);
    goto end_getattr;
  }
  
  struct TableEntry *f = findTableEntry(&file_tags, path_tags[s-1]);
  if (f) { // the path concerns a file
    for (int j = 0; j < s-1; j++) {
      if (!searchLabel(f->head, path_tags[j])) {
        res = -ENOENT;
        goto end_getattr;
      }
    }
    char * realpath = malloc(sizeof(char)*(strlen(dirpath)+strlen(path_tags[s-1])+2));
    realpath = tag_realpath(path);
    res = stat(realpath, stbuf);
    free(realpath);
  }
  else { // the path concerns a tag folder
    int h = 0;
    struct TableEntry *current, *tmp;
    HASH_ITER(hh, file_tags, current, tmp) {
      for (int j = 0; j < s; j++) {
        if (!searchLabel(current->head, path_tags[j])) {
          goto next_getattr;
        }
      }
      h = 1;
      next_getattr:
      continue;
    }
    if (h) res = stat(dirpath, stbuf);
    else res = -ENOENT;
  }
  
  end_getattr:
  for(int j = 0; j < s; j++) free(path_tags[j]);
  free(path_tags);
  LOG(" returning %d\n", res);
  return res;
}

/* list files within directory */
static int tag_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) 
{
  struct dirent *dirent;
  int i = 0;
  int j = 0;
  char ** tag_folders = malloc(sizeof(char*)*getTableSize(&tag_files));
  char ** path_tags = malloc(sizeof(char*)*(getTableSize(&tag_files) + 1));
  int s = tag_fillpathtags(path_tags, path); // get the tags in the path
  int hastags = 0;
  int nbfileindir = 0;
  
  struct TableEntry* f = NULL;
  struct Label* current = NULL;

  LOG("readdir '%s'\n", path);

  rewinddir(dir);
  while ((dirent = readdir(dir)) != NULL)
  {
    f = findTableEntry(&file_tags, dirent->d_name);
    current = NULL;
        
    // case of a file without tag when in the root folder
    if (!f && !strcmp(path, "/")) { filler(buf, dirent->d_name, NULL, 0); }
    // case of a file without tag when not in the root folder
    else if (!f) { continue; }
    // default case
    else {
      // test if the file has all the tags in the path
      hastags = 1;
      current = NULL;
      for (j = 0; j < s; j++) {
        hastags = 0;
        LL_FOREACH(f->head, current) {
          if (!strcmp(current->name, path_tags[j])) {
            hastags = 1;
            break;
          }
        }
        if (!hastags) break;
        current = NULL;
      }
      if (!hastags) continue;
      nbfileindir++;
      
      // display the file
      filler(buf, dirent->d_name, NULL, 0);
      
      // get its tags to create folders
      current = NULL;
      LL_FOREACH(f->head, current) {
        // add only the tags that are not already in the path
        for (j = 0; j < s; j++) {
          if (!strcmp(path_tags[j], current->name)) goto next;
        }
        // add only once
        for (j = 0; j < i; j++) {
          if (!strcmp(tag_folders[j], current->name)) goto next;
        }
        tag_folders[i] = malloc(sizeof(char)*strlen(current->name));
        strcpy(tag_folders[i], current->name);
        i++;
        next:
        continue;
      }
    }
  }

  // display the folders corresponding to the tags 
  for (j = 0; j < i; j++) {
    filler(buf, tag_folders[j], NULL, 0);
    free(tag_folders[j]);
  }

  free(tag_folders);
  for(j = 0; j < s; j++) free(path_tags[j]);
  free(path_tags);
  
  // if there is no file in the directory then it doesn't exist
  if (nbfileindir == 0) { LOG("no directory\n"); return -ENOENT; }

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
  int res = 0;
  
  LOG("link '%s'->'%s'\n", from, to);
  
  char ** path_tags = malloc(sizeof(char*)*(getTableSize(&tag_files) + 1));
  int s = tag_fillpathtags(path_tags, to); // get the tags in the path
  
  // the last string of path_tags is the name of the file
  if (findTableEntry(&file_tags, path_tags[s-1])) {
    for (int j = 0; j < s-1; j++) {
      // update the data structures
      addEntryLabel(&file_tags, path_tags[s-1], path_tags[j]);
      addTableEntry(&tag_files, path_tags[j]);
      addEntryLabel(&tag_files, path_tags[j], path_tags[s-1]);
      // TODO: update the .tags file
    }
  }
  else {
    res = -ENOENT;
  }
  
  for(int j = 0; j < s; j++) free(path_tags[j]);
  free(path_tags);
  return res;
}

/* change a tag into another 
 * example: mv foo/a.jpg baz/a.jpg 
 */
int tag_rename(const char* from, const char* to) {
  int res = 0;
  LOG("rename '%s'->'%s'\n", from, to);
  return res;
}

/* remove a tag from a file 
 * example: rm montag/a.jpg 
 */
int tag_unlink(const char* path) {
  int res = 0;
  LOG("unlink '%s'\n", path);
  return res;
}

/* create a new tag not yet bound to a file 
 * example: mkdir montag/
 */
int tag_mkdir(const char* path, mode_t mode) {
  int res = 0;
  LOG("mkdir '%s'\n", path);
  addTableEntry(&tag_files, (char *)path);
  // TODO: update the .tags file
  return res;
}

/* remove a non used tag 
 * example: rmdir montag
 */
int tag_rmdir(const char* path) {
  int res = 0;
  LOG("rmdir '%s'\n", path);
  return res;
}

static struct fuse_operations tag_oper = {
  .getattr = tag_getattr,
  .readdir = tag_readdir,
  .mkdir = tag_mkdir,
  .unlink = tag_unlink,
  .rmdir = tag_rmdir,
  .rename = tag_rename,
  .link = tag_link,
  .read = tag_read,
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
  
  LOG("starting tagfs in %s\n", dirpath);
  umask(0);
  err = fuse_main(argc, argv, &tag_oper, NULL);
  LOG("stopped tagfs with return code %d\n", err);

  closedir(dir);
  free(dirpath);

  LOG("\n");
  
  delTable(&tag_files);
  delTable(&file_tags);
  return err;
}
