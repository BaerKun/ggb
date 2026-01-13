#include "raylib.h"
#include "console.h"
#include "board.h"

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;

int main() {
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "ggb");
  SetTargetFPS(60);
  board_init();
  console_init();

  while (!WindowShouldClose()) {
    console_listen();
    BeginDrawing();
    ClearBackground(WHITE);
    board_draw_geom_objs();
    console_draw(0, WINDOW_HEIGHT - 100, WINDOW_WIDTH, 100);
    EndDrawing();
  }

  console_cleanup();
  board_cleanup();
  CloseWindow();
  return 0;
}