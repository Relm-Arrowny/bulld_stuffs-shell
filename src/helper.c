
#include "helper.h"
const char *builtins[] = {"exit", "echo", "type"};

int checkType(char* input){
    size_t count = sizeof(builtins) / sizeof(builtins[0]);

    for (size_t i = 0; i < count; i++){
        if (strcmp(input, builtins[i]) == 0) 
            return 1;
    }
    return 0;
}

char *checkTypePath(const char* path, const char* input){
    char ** path_list = split_string(path, ":");
    for (char **ptr = path_list; *ptr !=NULL; ptr++){
        
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", *ptr, input);
        if (access(full_path, X_OK) == 0){
            printf("%s is %s\n", input,full_path);
            return 1;
            }
    }
        
    free_string_list(path_list);
    return NULL;

}

char *checkTypeDefaultPath(const char* input){

    if (input == NULL) {
        return NULL;
    }
    const char *path_env = getenv("PATH");
    if (path_env == NULL || *path_env == '\0') {
        return NULL;
    }

    return checkTypePath(path_env, input);
}

int is_executable(const char *full_path) {
    if (access(full_path, X_OK) == 0) {
        return 1;
    }
    return 0;
}
void noCommand(char* com){
    printf("%s: command not found\n", com);
}

char **split_string(const char * str,const char *delim){
    if (str ==NULL || delim == NULL)
        return NULL;
    size_t capacity = 4;
    size_t count = 0;
 
    char **result = calloc(capacity , sizeof(char*));
    if (result == NULL) {
        return NULL;
    }
    char *str_copy = strdup(str);
    if (str_copy == NULL) {
        free(result);
        return NULL;
    }

    char *saveptr;
    char *token = strtok_r(str_copy, delim, &saveptr);

    while (token != NULL) {
        if (count+1 >= capacity){
            capacity *= 2;
            char **temp =realloc(result, capacity* sizeof(char*));
            if (temp ==NULL){
                free_string_list(result);
                free(str_copy);
                return temp;
            }
            result = temp;
        }
        result[count]= strdup(token);
        if (result[count] == NULL) {
            free_string_list(result);
            free(str_copy);
            return NULL;
        }
        token = strtok_r(NULL, delim, &saveptr);
        count++;
    }
    result[count] = NULL;
    free(str_copy);
    return result;
}

void free_string_list(char **list) {
    if (list == NULL) return;
    for (size_t i = 0; list[i] != NULL; i++) {
        free(list[i]);
    }
    free(list);
}