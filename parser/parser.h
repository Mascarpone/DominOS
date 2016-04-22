#ifndef PARSER_H
#define PARSER_H

#include "HashTable.h"
#include <stdio.h>

/**
 * extract from .tag all file/tags assosiations and arrange them into two tables files->tags and tags->files
 * @param fName     file to parse
 * @param file_tags where to store table of files
 * @param tag_files where to store table of tags
 */
void parse(char* fName, struct TableEntry** file_tags, struct TableEntry** tag_files);

#endif
