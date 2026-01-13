#include "raylib.h"
#include "console.h"
#include "commandline.h"
#include <string.h>

static int cursor_pos = 0;
static char commandline[CLI_BUF_SIZE] = {};

void console_init() {
  commandline_init();
}

void console_cleanup() {
  commandline_cleanup();
}

void console_listen() {
  for (int key; (key = GetCharPressed()) > 0;) {
    if (key >= 32 && key <= 126 && cursor_pos < CLI_BUF_SIZE - 1) {
      commandline[cursor_pos++] = (char)key;
    }
  }
  if (IsKeyPressed(KEY_BACKSPACE) && cursor_pos > 0) {
    commandline[--cursor_pos] = 0;
  }
  if (IsKeyPressed(KEY_ENTER)) {
    commandline_parse(commandline);
    memset(commandline, 0, cursor_pos);
    cursor_pos = 0;
  }
}

void console_draw(const int x, const int y, const int width, const int height) {
  DrawRectangle(x, y, width, height, LIGHTGRAY);
  DrawText(commandline, x, y, 30, BLACK);
}