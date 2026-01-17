#include "raylib.h"
#include "math_.h"
#include "board.h"
#include "object.h"

static struct {
  struct {
    Color point, circle, line;
  } color;
} default_option;

void board_init() {
  default_option.color.point = DARKBLUE;
  default_option.color.circle = GRAY;
  default_option.color.line = GRAY;
  object_module_init();
}

void board_cleanup() { object_module_cleanup(); }

static inline Color to_raylib_color(const int color) {
  // little-endian
  return (Color){(unsigned)color >> 16, (unsigned)color >> 8, color, 255};
}

static inline Vec2 get_end_point(const Vec2 p, const Vec2 q) {
  const Vec2 v = Vector2Subtract(q, p);
  const float norm = Vector2Length(v);
  if (norm == 0) return p;
  const float scale = 4096.f / norm;
  return (Vec2){p.x + v.x * scale, p.y + v.y * scale};
}

#define GET_RAYLIB_COLOR(type_, color_)                                        \
  ((color_) == -1 ? default_option.color.type_ : to_raylib_color(color_))

static void draw_circle(const GeomObject *obj) {
  DrawCircleLinesV(obj->pt1->coord,
                   vec2_distance(obj->pt1->coord, obj->pt2->coord),
                   GET_RAYLIB_COLOR(circle, obj->color));
}

static void draw_line(const GeomObject *obj) {
  const Vec2 p = obj->pt1->coord;
  const Vec2 q = obj->pt2->coord;
  switch (obj->type) {
  case SEG:
    DrawLineV(p, q, GET_RAYLIB_COLOR(line, obj->color));
    break;
  case RAY:
    DrawLineV(p, get_end_point(p, q), GET_RAYLIB_COLOR(line, obj->color));
    break;
  default:
    DrawLineV(get_end_point(p, q), get_end_point(q, p),
              GET_RAYLIB_COLOR(line, obj->color));
  }
}

static void draw_point(const GeomObject *obj) {
  DrawCircleV(obj->pt1->coord, 2, GET_RAYLIB_COLOR(point, obj->color));
}

#undef GET_DEFAULT_COLOR

void board_draw_geom_objs() {
  object_array_traverse(get_object_array(CIRCLE), draw_circle);
  object_array_traverse(get_object_array(LINE), draw_line);
  object_array_traverse(get_object_array(POINT), draw_point);
}