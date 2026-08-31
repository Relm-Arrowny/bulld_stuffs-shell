#ifndef COMMANDS_H
#define COMMANDS_H


int custom_echo(const char ** input);
int changeDir(const char * path);
int checkAndRun(char** com);
int checkType(const char** input);
int checkBuildinType(const char* input);
void noCommand(const char* com);

#endif