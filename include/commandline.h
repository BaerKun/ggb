#ifndef GGB_COMMAND_H
#define GGB_COMMAND_H

#include <stdio.h>

#define CMD_BUFF_SIZE 256

int read_line(FILE *stream, char *buffer);
int commandline_parse(char *line);

#endif //GGB_COMMAND_H