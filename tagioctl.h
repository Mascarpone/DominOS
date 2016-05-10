#ifndef PRINTTAGS_H
#define PRINTTAGS_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>

/* define arbitrary constants */
/* NOTE indeed, the memory passed to ioctl must be on the caller stack */
#define TAGS_MAXNB 100
#define TAGS_MAXLENGTH 100

/* ioctl shared structures */
struct tagslist {
  int tags_nb;
  char tags[TAGS_MAXNB][TAGS_MAXLENGTH];
};

/* ioctl shared codes */
#define IOC_GET_TAGS (_IOR('K', 0, struct tagslist))

#endif