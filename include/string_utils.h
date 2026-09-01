#ifndef STRING_UTILS_H
#define STRING_UTILS_H

char **split_string(const char *str, const char *delim);
char **split_string_quotes(const char *input);
void free_string_list(char **list);

#endif