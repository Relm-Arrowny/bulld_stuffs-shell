#ifndef COMMANDS_H
#define COMMANDS_H


typedef int (*builtin_func)(char **args);

typedef struct {
    const char *name;
    builtin_func func;    
}BuiltinCommand;

typedef struct {
    char * com;
    char * path;
}CompleteRegister;

const BuiltinCommand *find_command(const char * com);
extern const BuiltinCommand builtin_table[];

CompleteRegister *find_reg_complete(const char *com);

int custom_echo( char ** input);
int change_dir(char ** path);

int check_type(char** input);
int print_working_dir(char** input);
int custom_complete(char** input);


void noCommand(const char* com);
int custom_exit(char ** input);
int check_and_run(char** com);
#endif