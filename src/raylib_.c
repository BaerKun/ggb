#include "raylib.h"

typedef Vector2 Vec2;

void rl_init_window(int width, int height, const char *title) { InitWindow(width, height, title); }
void rl_close_window() { CloseWindow(); }
bool rl_window_should_close() { return WindowShouldClose(); }

void rl_clear_background(Color color) { ClearBackground(color); }
void rl_begin_drawing() { BeginDrawing(); }
void rl_end_drawing() { EndDrawing(); }

void rl_set_target_fps(int fps) { SetTargetFPS(fps); }
float rl_get_frame_time() { return GetFrameTime(); }

bool rl_is_key_pressed(int key) { return IsKeyPressed(key); }
int rl_get_char_pressed() { return GetCharPressed(); }
void rl_set_exit_key(int key) { SetExitKey(key); }

void rl_draw_line_v(Vec2 start, Vec2 end, Color color) { DrawLineV(start, end, color); }
void rl_draw_line_ex(Vec2 start, Vec2 end, float thick, Color color) { DrawLineEx(start, end, thick, color); }
void rl_draw_circle_v(Vec2 center, float radius, Color color) { DrawCircleV(center, radius, color); }
void rl_draw_circle_lines_v(Vec2 center, float radius, Color color) {DrawCircleLinesV(center, radius, color); }
void rl_draw_rectangle(int x, int y, int width, int height, Color color) { DrawRectangle(x, y, width, height, color); }
void rl_draw_rectangle_v(Vec2 pos, Vec2 size, Color color) { DrawRectangleV(pos, size, color); }
void rl_draw_text(const char *text, int x, int y, int size, Color color) { DrawText(text, x, y, size, color); }
