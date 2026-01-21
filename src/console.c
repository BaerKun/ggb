#include "raylib.h"
#include "console.h"
#include "commandline.h"
#include "message.h"
#include <string.h>

static int cursor_pos = 0;
static char commandline[CLI_BUF_SIZE] = {};
static struct {
  bool enable;
  Color color;
  char line[CLI_BUF_SIZE];
} feedback = {};

void console_init() { commandline_init(); }

void console_cleanup() { commandline_cleanup(); }

void console_listen() {
  for (int key; (key = GetCharPressed()) > 0;) {
    if (key >= 32 && key <= 126 && cursor_pos != CLI_BUF_SIZE) {
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
    feedback.enable = false;
  }
}

void console_draw(const int x, const int y, const int width, const int height) {
  DrawRectangle(x, y, width, height, LIGHTGRAY);
  DrawText(commandline, x + 5, y + 5, 30, BLACK);

  const Message *msg = message_pop();
  if (msg) {
    const int PREFIX_LEN = 8;
    feedback.enable = true;
    switch (msg->type & MSG_LEVEL_MASK) {
    case MSG_ERROR:
      feedback.color = RED;
      strcpy(feedback.line, "[ERROR] ");
      break;
    case MSG_WARN:
      feedback.color = (Color){245, 124, 0, 255};
      strcpy(feedback.line, "[WARN ] ");
      break;
    default:
      feedback.color = DARKGRAY;
      strcpy(feedback.line, "[INFO ] ");
    }
    strncpy(feedback.line + PREFIX_LEN, msg->content,
            CLI_BUF_SIZE - PREFIX_LEN);
  }
  message_make_empty();

  if (feedback.enable) {
    DrawText(feedback.line, x + 5, y + 40, 20, feedback.color);
  }
}