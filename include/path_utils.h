#ifndef PATH_UTILS_H
#define PATH_UTILS_H

char *check_typePath(const char* path, const char* input);
char *check_typeDefaultPath(const char* input);

int is_executable(const char *full_path);
int is_directory(const char *path) ;

#endif