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

#include "path_utils.h"
#include "redirection.h"

const BuiltinCommand builtin_table[] = {
    {"echo", wrapped_custom_echo},
    {"type", wrapped_check_type},
    {"cd"  , wrapped_change_dir},
    {"pwd", wrapped_print_working_dir},
    {"exit", custom_exit},
    {NULL,   NULL}
};

const BuiltinCommand *find_command(const char *com)
{
    for (int i = 0; builtin_table[i].name != NULL; i++){
        if(strcmp(com ,builtin_table[i].name)==0){
            return &builtin_table[i];
        }
    }
    return NULL;
}

int custom_echo(const char **input)
{    
    for (int i = 1; input[i] !=NULL; i++){
        printf("%s ", input[i]);
    }
    printf("\n");
    return 0;
}

int wrapped_custom_echo(char **input){
    return builtin_redirection_wrapper(input,custom_echo);
}

int change_dir(const char *path)
{  
    char target_path[PATH_MAX];
    if (path == NULL || strcmp(path, "~") == 0) {
        const char *home = getenv("HOME");
        if (home == NULL) {
            fputs("cd: HOME not set\n", stderr);
            return -1;
        }
        snprintf(target_path, sizeof(target_path), "%s", home);
    }
    else if (strncmp(path, "~/", 2) == 0) {
        char *home = getenv("HOME");
        if (home == NULL) {
            fputs("cd: HOME not set\n", stderr);
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

int wrapped_change_dir(char **input){
    return change_dir( input[1]);
}

int check_type(const char** input){
    if (input == NULL) return 0;
    char* result = NULL;
    if (check_builtin_type(input[1])){
        printf("%s is a shell builtin\n", input[1]);
        return 1;
    }
    else if((result = check_typeDefaultPath(input[1]))!= NULL){
        printf("%s is %s\n", input[1], result);
        free(result);
        return 1;
    }
    printf("%s: not found\n", input[1]);
    return 0;

}

 int wrapped_check_type(char** input){
    return builtin_redirection_wrapper(input,check_type);
 }

int check_builtin_type(const char* input){
    if (input == NULL) return 0;
    for (size_t i = 0; builtin_table[i].name != NULL; i++){
        if (strcmp(input, builtin_table[i].name) == 0) 
            return 1;
    }
    return 0;
}

int print_working_dir(const char** input){

    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
    printf("%s\n", cwd);
    } 
    else {
    perror("getcwd() error");
    }
    return 1;
}
int wrapped_print_working_dir(char** input){
     return builtin_redirection_wrapper(input,print_working_dir);

}



void noCommand(const char* com){
    printf("%s: command not found\n", com);
}

int custom_exit(char ** input){
    int code = 0;
    if (input[1] != NULL) {
        code = atoi(input[1]);
    }
    exit(code);

}


int check_and_run(char **com)
{
    char *path = NULL;
    

    if ((path =  check_typeDefaultPath(com[0]))!=NULL){
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

