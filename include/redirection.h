#ifndef REDIRECTION_H
#define REDIRECTION_H

void setup_redirection(char **com);
int builtin_redirection_wraper(char **com, int (*func)(const char **));

#endif