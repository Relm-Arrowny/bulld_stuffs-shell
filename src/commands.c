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
    {"echo", custom_echo},
    {"type", check_type},
    {"cd"  , change_dir},
    {"pwd", print_working_dir},
    {"exit", custom_exit},
    {"complete", custom_complete},
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
static int reg_size = 0;
static int reg_capacity = 10;
static CompleteRegister *comp_reg_table = NULL;

CompleteRegister *find_reg_complete(const char *com)
{
    if (comp_reg_table == NULL || com == NULL) return NULL;
    for (int i = 0; i < reg_size; i++) {
        if (comp_reg_table[i].com != NULL && strcmp(com, comp_reg_table[i].com) == 0) {
            return &comp_reg_table[i];
        }
    }
    return NULL;
}


int custom_echo(char **input)
{    
    for (int i = 1; input[i] !=NULL; i++){
        printf("%s ", input[i]);
    }
    printf("\n");
    return 0;
}



int change_dir(char **input)
{  
    char *path = input[1];
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

int check_type(char** input){
    if (input == NULL || input[1] == NULL)  return 0;
    char* result = NULL;
    
    if (find_command(input[1]) != NULL) {
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


int print_working_dir(char** input){

    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
    printf("%s\n", cwd);
    } 
    else {
    perror("getcwd() error");
    }
    return 1;
}

int custom_complete(char **input)
{
    if (comp_reg_table == NULL) {
        comp_reg_table = calloc(reg_capacity, sizeof(CompleteRegister));
    }
    if (strcmp(input[1], "-p")==0){
        CompleteRegister * temp_reg = find_reg_complete(input[2]);
        if (temp_reg!= NULL){
            printf("complete -C '%s' %s\n", temp_reg->path, temp_reg->com);
        }
        else{
            printf("complete: %s: no completion specification\n", input[2]);
        }
    }
    else if (strcmp(input[1], "-C")==0){
        if (input[2] == NULL || input[3] == NULL) {
            return 0; // Missing path or command name
        }

        if (reg_size >= reg_capacity-1){
            reg_capacity *= 2;
            CompleteRegister *temp = realloc(comp_reg_table, reg_capacity *
                 sizeof(CompleteRegister *));
            if (temp == NULL) {
                perror("realloc");
                return -1;
            }
            comp_reg_table = temp;
        }
        comp_reg_table[reg_size].path = strdup(input[2]);
        comp_reg_table[reg_size].com  = strdup(input[3]);
        reg_size++;
    }
    return 0;
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

