#ifndef HELPER_H
#define HELPER_H

char *checkTypePath(const char* path, const char* input);
char *checkTypeDefaultPath(const char* input);

int is_executable(const char *full_path);
int is_directory(const char *path) ;

void setup_redirection(char ** com);
int buldtin_redirection_wraper(char **com, int (*func)(const char **));

#endif