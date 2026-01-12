#include "raylib.h"
#include "core.h"
#include "console.h"
#include "board.h"

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;

int main() {
  core_init();
  SetTraceLogLevel(LOG_ERROR);
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "ggb");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    console_listen();
    BeginDrawing();
    ClearBackground(WHITE);
    board_draw();
    console_draw(0, WINDOW_HEIGHT - 100, WINDOW_WIDTH, 100);
    EndDrawing();
  }

  CloseWindow();
  core_release();
  return 0;
}