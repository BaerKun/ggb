#include "console.h"
#include "commandline.h"
#include <stdio.h>

void console_listen() {
  char buffer[CMD_BUFF_SIZE];

  while (1) {
    printf(">> ");

    if (read_line(stdin, buffer) || commandline_parse(buffer)) {
      return;
    }
  }
}