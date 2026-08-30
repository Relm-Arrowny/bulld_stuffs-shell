#ifndef HELPER_H
#define HELPER_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <linux/limits.h>

char *checkTypePath(const char* path, const char* input);
char *checkTypeDefaultPath(const char* input);

int is_executable(const char *full_path);
int is_directory(const char *path) ;

char **split_string(const char * str,const char *delim);
char **split_string_quotes(const char * input);
void free_string_list(char **list);

#endif