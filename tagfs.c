/*
 * Copyright (c) 2016 dominOS. All rights reserved.
 * File System by gcombes001, hdodelin, ohayak and flevern.
 */

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

static DIR *dir;
static char *dirpath;

/*******************
 * Logs
 */

#define LOGFILE "tagfs.log"
FILE *mylog;
#define LOG(args...) do { fprintf(mylog, args); fflush(mylog); } while (0)

/*******************
 * File operations
 */

/* get attributes */
/* Return file attributes. The "stat" structure is described in detail in the stat(2) manual page. 
   For the given pathname, this should fill in the elements of the "stat" structure. 
   If a field is meaningless or semi-meaningless (e.g., st_ino) then it should be set to 0 or given a "reasonable" value. 
   This call is pretty much required for a usable filesystem. */ 
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
  return 0;
}

/* read the content of the file */
int tag_read(const char *path, char *buffer, size_t len, off_t off, struct fuse_file_info *fi) {
  return 0;
}

static struct fuse_operations tag_oper = {
  .getattr = tag_getattr,
  .readdir = tag_readdir,
  .read = tag_read
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
  LOG("starting tagfs in %s\n", dirpath);
  err = fuse_main(argc, argv, &tag_oper, NULL);
  LOG("stopped tagfs with return code %d\n", err);

  closedir(dir);
  free(dirpath);

  LOG("\n");
  return err;
}
