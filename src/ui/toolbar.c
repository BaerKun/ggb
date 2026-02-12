#include "toolbar.h"
#include "raylib_.h"

const int TOOL_ICON_COUNT = 13;
const int TOOL_ICON_SIZE = 32;
const int TOOL_ICON_BORDER_X = 4;
const int TOOL_ICON_BORDER_Y = 4;

static struct {
  Rectangle window;
  Color bkg_color;

} toolbar;

void toolbar_init(const float x, const float y, const float w,
                  const float h) {
  toolbar.window = (Rectangle){x, y, w, h};
  toolbar.bkg_color = DARKGRAY;
}

void toolbar_cleanup() {

}

void toolbar_listen() {

}

void toolbar_draw() {
  rl_draw_rectangle_rec(toolbar.window, toolbar.bkg_color);

  int x = TOOL_ICON_BORDER_X;
  const int y = TOOL_ICON_BORDER_Y;
  for (int i = 0; i < TOOL_ICON_COUNT; i++) {
    rl_draw_rectangle(x, y, TOOL_ICON_SIZE, TOOL_ICON_SIZE, WHITE);
    x += TOOL_ICON_SIZE + TOOL_ICON_BORDER_X;
  }
}