#include "console.h"
#include "commandline.h"

void console_mode() {
  char buffer[CMD_BUFF_SIZE];
  commandline_init();

  while (1) {
    printf(">>  ");

    if (read_line(stdin, buffer) || commandline_parse(buffer)) {
      printf("Error\n");
      break;
    }
  }
  commandline_cleanup();
}