#include "board.h"
#include "object.h"
#include "raylib_.h"
#include "types.h"
#include <math.h>
#include <stdlib.h>

typedef struct {
  union {
    Vec2 pt;
    struct {
      Vec2 pt1, pt2;
    } ln;
    struct {
      Vec2 center;
      float radius;
    } cr;
    struct {
      Vec2 pos;
      char content[8];
    } tx;
  } data;
  Color color;
} DrawBuffer;

typedef struct {
  GeomSize cap, size;
  DrawBuffer *elems;
} DrawQueue;

struct {
  DrawQueue point, line, circle;
} draw_buffer;

static struct {
  struct {
    Color point, circle, line;
  } color;
} default_config;

static bool update_buffer;

static void draw_queue_init(DrawQueue *q, const GeomSize init_size) {
  q->cap = init_size;
  q->size = 0;
  q->elems = malloc(init_size * sizeof(DrawBuffer));
}

static void draw_queue_push(DrawQueue *q, const DrawBuffer elem) {
  if (q->size == q->cap) {
    q->cap *= 2;
    void *mem = realloc(q->elems, q->cap * sizeof(GeomObject));
    q->elems = mem;
  }
  q->elems[q->size++] = elem;
}

void board_init() {
  default_config.color.point = DARKBLUE;
  default_config.color.circle = GRAY;
  default_config.color.line = GRAY;
  draw_queue_init(&draw_buffer.point, 64);
  draw_queue_init(&draw_buffer.circle, 16);
  draw_queue_init(&draw_buffer.line, 32);
  object_module_init();
}

void board_cleanup() { object_module_cleanup(); }

void board_update_buffer() { update_buffer = true; }

static inline bool is_default_color(const Color color) {
  return color.a == 0;
}

static inline Vec2 get_end_point(const Vec2 p, const Vec2 q) {
  const Vec2 v = {q.x - p.x, q.y - p.y};
  const float norm = sqrtf(v.x * v.x + v.y * v.y);
  if (norm == 0) return p;
  const float scale = 4096.f / norm;
  return (Vec2){p.x + v.x * scale, p.y + v.y * scale};
}

static inline float vec2_dist(const Vec2 p, const Vec2 q) {
  const float dx = p.x - q.x;
  const float dy = p.y - q.y;
  return sqrtf(dx * dx + dy * dy);
}

static void get_draw_buffer(const GeomObject *obj) {
  DrawBuffer buff;
  switch (obj->type) {
  case POINT:
    buff.data.pt = point_get_coord(obj->pt1);
    buff.color =
        is_default_color(obj->color) ? default_config.color.point : obj->color;
    draw_queue_push(&draw_buffer.point, buff);
    break;
  case CIRCLE:
    buff.data.cr.center = point_get_coord(obj->pt1);
    buff.data.cr.radius =
        vec2_dist(buff.data.cr.center, point_get_coord(obj->pt2));
    buff.color =
        is_default_color(obj->color) ? default_config.color.circle : obj->color;
    draw_queue_push(&draw_buffer.circle, buff);
    break;
  case SEG:
    buff.data.ln.pt1 = point_get_coord(obj->pt1);
    buff.data.ln.pt2 = point_get_coord(obj->pt2);
    buff.color =
        is_default_color(obj->color) ? default_config.color.line : obj->color;
    draw_queue_push(&draw_buffer.line, buff);
    break;
  case RAY:
    buff.data.ln.pt1 = point_get_coord(obj->pt1);
    buff.data.ln.pt2 =
        get_end_point(buff.data.ln.pt1, point_get_coord(obj->pt2));
    buff.color =
        is_default_color(obj->color) ? default_config.color.line : obj->color;
    draw_queue_push(&draw_buffer.line, buff);
    break;
  case LINE: {
    const Vec2 p1 = point_get_coord(obj->pt1);
    const Vec2 p2 = point_get_coord(obj->pt2);
    buff.data.ln.pt1 = get_end_point(p1, p2);
    buff.data.ln.pt2 = get_end_point(p2, p1);
    buff.color =
        is_default_color(obj->color) ? default_config.color.line : obj->color;
    draw_queue_push(&draw_buffer.line, buff);
  }
  default:
    break;
  }
}

void board_draw_update() {
  if (update_buffer) {
    draw_buffer.point.size = draw_buffer.circle.size = draw_buffer.line.size = 0;
    object_traverse(get_draw_buffer);
    update_buffer = false;
  }

  for (GeomId i = 0; i < draw_buffer.point.size; i++) {
    const DrawBuffer buff = draw_buffer.point.elems[i];
    rl_draw_circle_v(buff.data.pt, 2, buff.color);
  }

  for (GeomId i = 0; i < draw_buffer.circle.size; i++) {
    const DrawBuffer buff = draw_buffer.circle.elems[i];
    rl_draw_circle_lines_v(buff.data.cr.center, buff.data.cr.radius,
                           buff.color);
  }

  for (GeomId i = 0; i < draw_buffer.line.size; i++) {
    const DrawBuffer buff = draw_buffer.line.elems[i];
    rl_draw_line_v(buff.data.ln.pt1, buff.data.ln.pt2, buff.color);
  }
}