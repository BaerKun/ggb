#ifndef GGB_COMMANDLINE_H
#define GGB_COMMANDLINE_H

#include <stdio.h>

#define CLI_BUF_SIZE 256

void commandline_init();
int read_line(FILE *stream, char *buffer);
int commandline_parse(char *line);
void commandline_cleanup();

#endif //GGB_COMMANDLINE_H