#include "raylib.h"
#include "types.h"
#include "board.h"
#include "object.h"
#include <stdlib.h>

static struct {
  struct {
    Color point, circle, line;
  } color;
} default_option;

typedef struct {
  GeomSize cap, size;
  const GeomObject **elems;
} DrawQueue;

struct {
  DrawQueue point, circle, line;
} draw_queue;

static void draw_queue_init(DrawQueue *q, const GeomSize init_size) {
  q->cap = init_size;
  q->size = 0;
  q->elems = malloc(init_size * sizeof(GeomObject));
}

static void draw_queue_push(DrawQueue *q, const GeomObject *obj) {
  if (q->size == q->cap) {
    q->cap *= 2;
    void *mem = realloc(q->elems, q->cap * sizeof(GeomObject));
    q->elems = mem;
  }
  q->elems[q->size++] = obj;
}

void board_init() {
  default_option.color.point = DARKBLUE;
  default_option.color.circle = GRAY;
  default_option.color.line = GRAY;
  draw_queue_init(&draw_queue.point, 128);
  draw_queue_init(&draw_queue.circle, 64);
  draw_queue_init(&draw_queue.line, 64);
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

static void get_draw_queue(const GeomObject *obj) {
  if (!obj->show) return;
  switch (obj->type) {
  case POINT:
    draw_queue_push(&draw_queue.point, obj);
    break;
  case CIRCLE:
    draw_queue_push(&draw_queue.circle, obj);
    break;
  default:
    draw_queue_push(&draw_queue.line, obj);
  }
}

void board_draw_update() {
#define GET_RAYLIB_COLOR(type_, color_)                                        \
  ((color_) == -1 ? default_option.color.type_ : to_raylib_color(color_))

  draw_queue.point.size = draw_queue.circle.size = draw_queue.line.size = 0;
  object_traverse(get_draw_queue);

  for (GeomId i = 0; i < draw_queue.point.size; i++) {
    const GeomObject *obj = draw_queue.point.elems[i];
    DrawCircleV(point_get_coord(obj->pt1), 2,
                GET_RAYLIB_COLOR(point, obj->color));
  }
  for (GeomId i = 0; i < draw_queue.circle.size; i++) {
    const GeomObject *obj = draw_queue.circle.elems[i];
    const Vec2 pt1 = point_get_coord(obj->pt1);
    const Vec2 pt2 = point_get_coord(obj->pt2);
    DrawCircleLinesV(pt1, vec2_distance(pt1, pt2),
                     GET_RAYLIB_COLOR(circle, obj->color));
  }
  for (GeomId i = 0; i < draw_queue.line.size; i++) {
    const GeomObject *obj = draw_queue.line.elems[i];
    const Vec2 pt1 = point_get_coord(obj->pt1);
    const Vec2 pt2 = point_get_coord(obj->pt2);
    switch (obj->type) {
    case SEG:
      DrawLineV(pt1, pt2, GET_RAYLIB_COLOR(line, obj->color));
      break;
    case RAY:
      DrawLineV(pt1, get_end_point(pt1, pt2),
                GET_RAYLIB_COLOR(line, obj->color));
      break;
    default:
      DrawLineV(get_end_point(pt1, pt2), get_end_point(pt2, pt1),
                GET_RAYLIB_COLOR(line, obj->color));
    }
  }
#undef GET_DEFAULT_COLOR
}