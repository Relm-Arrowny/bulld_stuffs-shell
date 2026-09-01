#ifndef COMMANDS_H
#define COMMANDS_H


int custom_echo(const char ** input);
int change_dir(const char * path);
int check_and_run(char** com);
int check_type(const char** input);
int check_builtin_type(const char* input);
void noCommand(const char* com);

#endif