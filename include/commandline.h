#ifndef GGB_COMMAND_H
#define GGB_COMMAND_H

#include <stdio.h>

#define CMD_BUFF_SIZE 256

void commandline_init();
int read_line(FILE *stream, char *buffer);
int commandline_parse(char *line);
void commandline_cleanup();

#endif //GGB_COMMAND_H