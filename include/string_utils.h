#ifndef STRING_UTILS_H
#define STRING_UTILS_H

typedef struct {
    char **items;
    int count;
    int capacity;
} StringList;

void init_string_list(StringList *list);
void add_to_string_list(StringList *list, const char *str);
void sort_string_list(StringList *list);
void rm_dup_string_list(StringList *list);
void free_string_list(char **list);
char ** increase_string_list_capacity(char **list, int *capacity, int current_size);

char **split_string(const char *str, const char *delim);
char **split_string_quotes(const char *input);

#endif