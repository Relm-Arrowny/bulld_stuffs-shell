
#include "helper.h"
const char *builtins[] = {"exit", "echo", "type", "pwd"};


int checkType(const char* input){
    if (input == NULL) return 0;
    char* result = NULL;
    if (checkBuildinType(input)){
        printf("%s is a shell builtin\n", input);
        return 1;
    }
    else if(result = checkTypeDefaultPath(input)){
        printf("%s is %s\n", input, result);
        free(result);
        return 1;
    }
    printf("%s: not found\n", input);
    return 0;

}

int checkBuildinType(const char* input){
    if (input == NULL) return 0;
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
            char * return_path =strdup(full_path);
            free_string_list(path_list);
            return return_path;
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


int checkAndRun(const char *com)
{
    char ** split_com =  split_string(com, " \t\r\n");
    if (split_com == NULL || split_com[0] == NULL) {
        free_string_list(split_com);
        return 0;
    }
    char *path = NULL;
    

    if ((path =  checkTypeDefaultPath(split_com[0]))!=NULL){
    }
    else if(is_executable(split_com[0])){
            path = strdup(split_com[0]);
    }
    else {
        noCommand(com);
        free_string_list(split_com);
        return 0;
    }
    if (path !=NULL){
        pid_t pid = fork();
        if (pid==0){
            execv(path, split_com);
            perror("execv failed");
            free(path);
            free_string_list(split_com);
            exit(0);
        }
        
        else{
            int status;
            waitpid(pid, &status, 0);

            free(path);
            free_string_list(split_com);
        }

    }
    return 1;
}


int is_executable(const char *full_path) {
    if (access(full_path, X_OK) == 0) {
        return 1;
    }
    return 0;
}
void noCommand(const char* com){
    printf("%s: command not found\n", com);
}

int changeDir(const char *path)
{  
    if (is_directory(path))
        chdir(path);
    else
        printf("cd: %s: No such file or directory\n", path);

}

int is_directory(const char *path) {
    struct stat path_stat;
    

    if (stat(path, &path_stat) != 0) {
        return 0;
    }
    
    return S_ISDIR(path_stat.st_mode);
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