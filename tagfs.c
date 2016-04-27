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
static char *tagpath;
struct TableEntry * tag_files = NULL;
struct TableEntry * file_tags = NULL;

/*******************
 * Useful functions
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

/*******************
 * Fuse operations
 */

/* get attributes */
static int tag_getattr(const char *path, struct stat *stbuf) {
  int res = 0;
  char ** path_tags = malloc(sizeof(char*)*(getTableSize(&tag_files) + 1));
  int s = tag_fillpathtags(path_tags, path);

  LOG("getattr '%s'\n", path);
  
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
    
    if (h) // a file has all the tags
      res = stat(dirpath, stbuf); 
    else if (s == 1 && findTableEntry(&tag_files, path_tags[0])) // the tag has no file referencing it
      res = stat(dirpath, stbuf); 
    else 
      res = -ENOENT;
  }
  
  end_getattr:
  for(int j = 0; j < s; j++) free(path_tags[j]);
  free(path_tags);
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
  
  // if we are in the root folder, add the tag that have no files
  if (!strcmp(path, "/")) { 
    HASH_ITER(hh, tag_files, current_file, tmp) {
      if (!countLabels(current_file->head)) {
        addTableEntry(&tag_folders, current_file->name);
      }
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
  int res = 0;
  
  LOG("link '%s'->'%s'\n", from, to);
  
  char ** path_tags = malloc(sizeof(char*)*(getTableSize(&tag_files) + 1));
  int s = tag_fillpathtags(path_tags, to); // get the tags in the path
  
  // the last string of path_tags is the name of the file
  if (!strcmp(from+1, path_tags[s-1])) {
    if (findTableEntry(&file_tags, path_tags[s-1])) {
      for (int j = 0; j < s-1; j++) {
        if (findTableEntry(&tag_files, path_tags[j])) {
          // update the data structures
          addEntryLabel(&file_tags, path_tags[s-1], path_tags[j]);
          addEntryLabel(&tag_files, path_tags[j], path_tags[s-1]);
        }
        else {
          res = -ENOENT;
          continue;
        }
      }
    }
    else {
      res = -ENOENT;
    }
  }
  else {
    LOG("cannot rename a file\n");
    res = -EPERM;
  }
  
  // update the ./tags file
  updateTags(tagpath, &file_tags);
  
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
  
  char ** path_tags_from = malloc(sizeof(char*)*(getTableSize(&tag_files) + 1));
  int s_from = tag_fillpathtags(path_tags_from, from);
  char ** path_tags_to = malloc(sizeof(char*)*(getTableSize(&tag_files) + 1));
  int s_to = tag_fillpathtags(path_tags_to, to);
  
  if (s_from == 2 && s_to == 2 && !strcmp(path_tags_to[1], path_tags_from[1])) {
    struct TableEntry *f = findTableEntry(&file_tags, path_tags_from[1]);
    
    struct TableEntry *t_to = findTableEntry(&tag_files, path_tags_to[0]);
    addLabel(&t_to->head, path_tags_to[1]);
    addLabel(&f->head, path_tags_to[0]);
    
    struct TableEntry *t_from = findTableEntry(&tag_files, path_tags_from[0]);
    delLabel(&t_from->head, path_tags_from[1]);
    delLabel(&f->head, path_tags_from[0]);  
  }
  else {
    LOG("rename syntax not corresponding to the protocole\n");
    res = -EPERM;
  }
  
  // update the ./tags file
  updateTags(tagpath, &file_tags);
  
  for(int j = 0; j < s_to; j++) free(path_tags_to[j]);
  free(path_tags_to);
  for(int j = 0; j < s_from; j++) free(path_tags_from[j]);
  free(path_tags_from);
  return res;
}

/* remove a tag from a file 
 * example: rm montag/a.jpg 
 */
int tag_unlink(const char* path) {
  int res = 0;
  
  LOG("unlink '%s'\n", path);
  
  char ** path_tags = malloc(sizeof(char*)*(getTableSize(&tag_files) + 1));
  int s = tag_fillpathtags(path_tags, path); // get the tags in the path
  
  if (s >= 2) {
    struct TableEntry *f = findTableEntry(&file_tags, path_tags[s-1]);
    struct TableEntry *t = NULL;
    for (int j = 0; j < s-1; j++) {
      t = findTableEntry(&tag_files, path_tags[j]);
      delLabel(&t->head, path_tags[s-1]);
      delLabel(&f->head, path_tags[j]);
    }
  } 
  else {
    LOG("cannot remove file in the root folder\n");
    res = -EPERM;
  } 
  
  // update the ./tags file
  updateTags(tagpath, &file_tags);
  
  for(int j = 0; j < s; j++) free(path_tags[j]);
  free(path_tags);
  return res;
}

/* create a new tag not yet bound to a file 
 * example: mkdir montag/
 */
int tag_mkdir(const char* path, mode_t mode) {
  int res = 0;
  LOG("mkdir '%s'\n", path);
  addTableEntry(&tag_files, (char *)(path+1));
  return res;
}

/* remove a non used tag 
 * example: rmdir montag
 */
int tag_rmdir(const char* path) {
  int res = 0;
  struct TableEntry *current, *tmp;
  LOG("rmdir '%s'\n", path);
  delTableEntry(&tag_files, (char *)(path+1));
  HASH_ITER(hh, file_tags, current, tmp) {
    delLabel(&current->head, (char *)(path+1));
  }
  
  // update the ./tags file
  updateTags(tagpath, &file_tags);
  
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
  
  // parse the .tags file to init global hash tables
  char *tagfilepath = malloc(sizeof(char)*(strlen(dirpath)+11));
  strcpy(tagfilepath, dirpath);
  strcat(tagfilepath, "/.tags"); 
  parse(tagfilepath, &file_tags, &tag_files);
  
  // set the global variable indicating the path of the ./tags that that must be updated
  strcat(tagfilepath, ".new");
  tagpath = tagfilepath;
  
  // complete the file_tags hashtable with the files that don't have a tag
  struct dirent *dirent;
  rewinddir(dir);
  while ((dirent = readdir(dir)) != NULL) {
    if (dirent->d_type == DT_REG && strncmp(dirent->d_name, ".tags", 5)) {
      if(!findTableEntry(&file_tags, dirent->d_name)) {
        addTableEntry(&file_tags, dirent->d_name);
      }
    }
  }
  
  LOG("\nstarting tagfs in %s\n", dirpath);
  umask(0);
  err = fuse_main(argc, argv, &tag_oper, NULL);
  LOG("stopped tagfs with return code %d\n\n", err);

  closedir(dir);
  free(dirpath);
  free(tagfilepath);
  delTable(&tag_files);
  delTable(&file_tags);
  return err;
}
