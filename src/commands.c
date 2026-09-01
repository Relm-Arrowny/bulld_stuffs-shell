#include "commands.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <fcntl.h>

#include "helper.h"
const char *builtins[] = {"exit", "echo", "type", "pwd"};

int custom_echo(const char **input)
{
    for (int i = 1; input[i] !=NULL; i++){
        printf("%s ", input[i]);
    }
    printf("\n");
    return 0;
}

int changeDir(const char *path)
{  
    char target_path[PATH_MAX];
    if (path == NULL || strcmp(path, "~") == 0) {
        char *home = getenv("HOME");
        if (home == NULL) {
            fprintf(stderr, "cd: HOME not set\n");
            return -1;
        }
        snprintf(target_path, sizeof(target_path), "%s", home);
    }
    else if (strncmp(path, "~/", 2) == 0) {
        char *home = getenv("HOME");
        if (home == NULL) {
            fprintf(stderr, "cd: HOME not set\n");
            return -1;
        }
        snprintf(target_path, sizeof(target_path), "%s/%s", home, path + 2);
    }
    else if (is_directory(path)){
        snprintf(target_path, sizeof(target_path), "%s", path);
    }
    else{
        printf("cd: %s: No such file or directory\n", path);
        return -1;
    }
    if (chdir(target_path) != 0) {
        
        perror("cd");
        return -1;
    }

    return 0;

}


int checkAndRun(char **com)
{
    char *path = NULL;
    

    if ((path =  checkTypeDefaultPath(com[0]))!=NULL){
    }
    else if(is_executable(com[0])){
            path = strdup(com[0]);
    }
    else {
        noCommand(com[0]);
        return 0;
    }
    if (path !=NULL){
        pid_t pid = fork();
        if (pid==0){
            setup_redirection(com);
            execv(path, com);
            perror("execv failed");
            free(path);
            exit(127);
        }
        
        else{
            int status;
            waitpid(pid, &status, 0);
            free(path);
        }

    }
    return 1;
}

int checkType(const char** input){
    if (input == NULL) return 0;
    char* result = NULL;
    if (checkBuildinType(input[1])){
        printf("%s is a shell builtin\n", input[1]);
        return 1;
    }
    else if((result = checkTypeDefaultPath(input[1]))!= NULL){
        printf("%s is %s\n", input[1], result);
        free(result);
        return 1;
    }
    printf("%s: not found\n", input[1]);
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

void noCommand(const char* com){
    printf("%s: command not found\n", com);
}

