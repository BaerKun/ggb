#include "raylib.h"
#include "object.h"
#include "commandline.h"

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;

static void core_init() {
  commandline_init();
  object_module_init(256);
}

static void core_release() {
  commandline_cleanup();
  object_module_cleanup();
}

int main() {
  core_init();
  SetTraceLogLevel(LOG_ERROR);
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "ggb");

  char buff[CLI_BUF_SIZE];
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(WHITE);
    object_draw_all();
    EndDrawing();
    read_line(stdin, buff);
    commandline_parse(buff);
  }

  CloseWindow();
  core_release();
  return 0;
}