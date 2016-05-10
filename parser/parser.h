#ifndef PARSER_H
#define PARSER_H

#include "HashTable.h"
#include <stdio.h>

/**
 * extract from .tags all file/tags assosiations and arrange them into two tables files->tags and tags->files
 * @param fName     file to parse
 * @param file_tags where to store table of files
 * @param tag_files where to store table of tags
 */
void parse(char* fName, struct TableEntry** file_tags, struct TableEntry** tag_files);

/**
 * rewrite .tags file to match with the file->tags structure
 * @param fname     file to be written
 * @param file_tags data structure to be copied from
 */
void updateTags(char *fname, struct TableEntry** file_tags);

#endif
