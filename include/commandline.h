#ifndef GGB_COMMANDLINE_H
#define GGB_COMMANDLINE_H

#define CLI_BUF_SIZE 256

void commandline_init();
int commandline_parse(char *line);
void commandline_cleanup();

#endif //GGB_COMMANDLINE_H