#include "readline.h"

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

static struct termios orig_termios;
static int atexit_registered = 0;
static int is_raw_mode = 0;

void cooked_mode() {
    if (!is_raw_mode) {
        return;
    }
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    is_raw_mode = 0;
}

void raw_mode(){
    if (is_raw_mode){
        return;

    }
    struct termios  raw;
    if (!atexit_registered) {
        atexit(cooked_mode);
        atexit_registered = 1;
    }
    tcgetattr(STDIN_FILENO, &orig_termios);
    raw = orig_termios;

    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN]  = 1;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    is_raw_mode = 1;
}

char *readline(const char *prompt) {
    raw_mode();
    char *buffer = NULL;
    int len = 0;
    buffer = malloc(4096);
    if (buffer == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    printf("%s", prompt);
    char c;
    do {
        ssize_t bytes_read = read(STDIN_FILENO, &c, 1);
        if (bytes_read < 0) {
            perror("read");
            free(buffer);
            exit(EXIT_FAILURE);
        }
        if (c != '0') {
            buffer[len++] = c;
            buffer[len] = '\0';
        }
        printf("$%s",buffer);
    }while (c != '\r' && c != '\n');
    cooked_mode();
    return buffer;
}