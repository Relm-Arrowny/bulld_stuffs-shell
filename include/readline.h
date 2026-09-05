#ifndef READLINE_H
#define READLINE_H

void raw_mode();
void cooked_mode();
char *readline(const char *prompt);

#endif