#include "console.h"
#include "commandline.h"
#include "object.h"
#include "board.h"

void console_mode() {
  char buffer[CMD_BUFF_SIZE];
  commandline_init();
  object_module_init(256);
  board_init();

  while (1) {
    printf(">>  ");

    if (read_line(stdin, buffer) || commandline_parse(buffer)) {
    }
    board_refresh();
  }

  board_cleanup();
  object_module_cleanup();
  commandline_cleanup();
}