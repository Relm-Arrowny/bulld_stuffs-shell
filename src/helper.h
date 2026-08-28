#ifndef HELPER_H
#define HELPER_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int checkType(char* input);
char *checkTypePath(const char* path, const char* input);
char *checkTypeDefaultPath(const char* input);
void noCommand(char* com);

char **split_string(const char * str,const char *delim);
void free_string_list(char **list);

#endif