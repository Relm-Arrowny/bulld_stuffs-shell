#ifndef REDIRECTION_H
#define REDIRECTION_H

void setup_redirection(char **com);
int buildtin_redirection_wraper(char **com, int (*func)(const char **));

#endif