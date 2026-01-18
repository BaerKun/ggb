#include "raylib.h"
#include "types.h"
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
  const Vec2 pt1 = point_get_coord(obj->pt1);
  const Vec2 pt2 = point_get_coord(obj->pt2);
  DrawCircleLinesV(pt1, vec2_distance(pt1, pt2),
                   GET_RAYLIB_COLOR(circle, obj->color));
}

static void draw_line(const GeomObject *obj) {
  const Vec2 pt1 = point_get_coord(obj->pt1);
  const Vec2 pt2 = point_get_coord(obj->pt2);
  switch (obj->type) {
  case SEG:
    DrawLineV(pt1, pt2, GET_RAYLIB_COLOR(line, obj->color));
    break;
  case RAY:
    DrawLineV(pt1, get_end_point(pt1, pt2), GET_RAYLIB_COLOR(line, obj->color));
    break;
  default:
    DrawLineV(get_end_point(pt1, pt2), get_end_point(pt2, pt1),
              GET_RAYLIB_COLOR(line, obj->color));
  }
}

static void draw_point(const GeomObject *obj) {
  DrawCircleV(point_get_coord(obj->pt1), 2,
              GET_RAYLIB_COLOR(point, obj->color));
}

#undef GET_DEFAULT_COLOR

void board_draw_geom_objs() {
  object_array_traverse(get_object_array(CIRCLE), draw_circle);
  object_array_traverse(get_object_array(LINE), draw_line);
  object_array_traverse(get_object_array(POINT), draw_point);
}