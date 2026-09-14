#ifndef COMMANDS_H
#define COMMANDS_H


typedef int (*builtin_func)(char **args);

typedef struct {
    const char *name;
    builtin_func func;    
}BuiltinCommand;

const BuiltinCommand *find_command(const char * com);

extern const BuiltinCommand builtin_table[];

int custom_echo(const char ** input);
int wrapped_custom_echo(char **input);

int change_dir(const char * path);
int wrapped_change_dir(char **input);


int check_type(const char** input);
int wrapped_check_type(char** input);

int print_working_dir(const char** input);
int wrapped_print_working_dir(char** input);

int check_builtin_type(const char* input);
void noCommand(const char* com);

int custom_exit(char ** input);

int check_and_run(char** com);
#endif