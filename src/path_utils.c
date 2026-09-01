
#include "path_utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#include "string_utils.h"

char *check_typePath(const char* path, const char* input){
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

char *check_typeDefaultPath(const char* input){

    if (input == NULL) {
        return NULL;
    }
    const char *path_env = getenv("PATH");
    if (path_env == NULL || *path_env == '\0') {
        return NULL;
    }

    return check_typePath(path_env, input);
}


int is_executable(const char *full_path) {
    if (access(full_path, X_OK) == 0) {
        return 1;
    }
    return 0;
}

int is_directory(const char *path) {
    struct stat path_stat;
    

    if (stat(path, &path_stat) != 0) {
        return 0;
    }
    
    return S_ISDIR(path_stat.st_mode);
}
