#include "raylib_.h"
#include "console.h"
#include "board.h"

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;

int main() {
  rl_init_window(WINDOW_WIDTH, WINDOW_HEIGHT, "ggb");
  rl_set_target_fps(60);
  board_init();
  console_init();

  while (!rl_window_should_close()) {
    console_listen();
    rl_begin_drawing();
    rl_clear_background(WHITE);
    board_draw_update();
    console_draw(0, WINDOW_HEIGHT - 100, WINDOW_WIDTH, 100);
    rl_end_drawing();
  }

  console_cleanup();
  board_cleanup();
  rl_close_window();
  return 0;
}