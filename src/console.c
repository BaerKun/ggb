#include "console.h"
#include "board.h"
#include "commandline.h"
#include "message.h"
#include "raylib_.h"
#include <string.h>

typedef struct {
  bool enable;
  Color color;
  char line[CLI_BUF_SIZE];
} Feedback;

static struct {
  Rectangle window;
  Font font;
  int cursor_pos;
  char commandline[CLI_BUF_SIZE];
  Feedback feedback;
} console;

void console_init(const float x, const float y, const float width,
                  const float height) {
  console.window = (Rectangle){x, y, width, height};
  console.font = rl_get_font_default();
  commandline_init();
}
void console_cleanup() { commandline_cleanup(); }

void console_listen() {
  for (int key; (key = rl_get_char_pressed()) > 0;) {
    if (key >= 32 && key <= 126 && console.cursor_pos != CLI_BUF_SIZE) {
      console.commandline[console.cursor_pos++] = (char)key;
    }
  }
  if (rl_is_key_pressed(KEY_BACKSPACE) && console.cursor_pos > 0) {
    console.commandline[--console.cursor_pos] = 0;
  }
  if (rl_is_key_pressed(KEY_ENTER)) {
    commandline_parse(console.commandline);
    memset(console.commandline, 0, console.cursor_pos);
    console.cursor_pos = 0;
    console.feedback.enable = false;
    board_update_buffer();
  }
}

void console_draw() {
  rl_draw_rectangle_rec(console.window, LIGHTGRAY);
  rl_draw_text_ex(console.font, console.commandline,
                  (Vector2){console.window.x + 5, console.window.y + 5}, 30, 2,
                  BLACK);

  const Message *msg = message_pop();
  if (msg) {
    const int PREFIX_LEN = 8;
    console.feedback.enable = true;
    switch (msg->level) {
    case MSG_ERROR:
      console.feedback.color = RED;
      strcpy(console.feedback.line, "[ERROR] ");
      break;
    case MSG_WARN:
      console.feedback.color = (Color){245, 124, 0, 255};
      strcpy(console.feedback.line, "[WARN ] ");
      break;
    default:
      console.feedback.color = DARKGRAY;
      strcpy(console.feedback.line, "[INFO ] ");
    }
    strncpy(console.feedback.line + PREFIX_LEN, msg->content,
            CLI_BUF_SIZE - PREFIX_LEN);
  }
  message_make_empty();

  if (console.feedback.enable) {
    rl_draw_text_ex(console.font, console.feedback.line,
                    (Vector2){console.window.x + 5, console.window.y + 40}, 20,
                    1, console.feedback.color);
  }
}