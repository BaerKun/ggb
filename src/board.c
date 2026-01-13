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
}

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

void board_draw_geom_objs() {
#define GET_RAYLIB_COLOR(type_, color_) ((color_) == -1 ? default_option.color.type_ : to_raylib_color(color_))

  const GeomSparseArray *circles = get_object_array(CIRCLE);
  for (int i = 0, n = 0; i < circles->cap && n < circles->size; ++i) {
    if (circles->state[i] == -1) {
      n++;
      const GeomObject *obj = circles->data + i;
      DrawCircleLinesV(obj->pt1->coord,
                       vec2_distance(obj->pt1->coord, obj->pt2->coord),
                       GET_RAYLIB_COLOR(circle, obj->color));
    }
  }

  const GeomSparseArray *lines = get_object_array(LINE);
  for (int i = 0, n = 0; i < lines->cap && n < lines->size; ++i) {
    if (lines->state[i] == -1) {
      n++;
      const GeomObject *obj = lines->data + i;
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
  }

  const GeomSparseArray *points = get_object_array(POINT);
  for (int i = 0, n = 0; i < points->cap && n < points->size; i++) {
    if (points->state[i] == -1) {
      n++;
      const GeomObject *obj = points->data + i;
      DrawCircleV(obj->pt1->coord, 2, GET_RAYLIB_COLOR(point, obj->color));
    }
  }

#undef GET_DEFAULT_COLOR
}