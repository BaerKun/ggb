#include "board.h"
#include "object.h"
#include "raylib_.h"
#include <stdlib.h>

typedef Vector2 Vec2;

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

static struct {
  Rectangle window;
  bool should_update_buffer;
  float xform_scale;
  Vec2 xform_translate;
  float xform_rotate[2][2];
  struct {
    Color point, circle, line;
  } default_color;
  struct {
    DrawQueue point, line, circle;
  } draw_queue;

} board;

static void draw_queue_init(DrawQueue *q, GeomSize init_size);
static void draw_queue_push(DrawQueue *q, DrawBuffer elem);
static void get_draw_buffer(const GeomObject *obj);

void board_init(const float x, const float y, const float width,
                const float height) {
  board.default_color.point = DARKBLUE;
  board.default_color.circle = GRAY;
  board.default_color.line = GRAY;

  board.window = (Rectangle){x, y, width, height};
  board.xform_scale = 1.f;
  board.xform_translate.y = height;
  board.xform_rotate[0][0] = 1.f;
  board.xform_rotate[1][1] = -1.f;

  draw_queue_init(&board.draw_queue.point, 64);
  draw_queue_init(&board.draw_queue.circle, 16);
  draw_queue_init(&board.draw_queue.line, 32);
  object_module_init();
}

void board_cleanup() { object_module_cleanup(); }

void board_update_buffer() { board.should_update_buffer = true; }

void board_draw_update() {
  if (board.should_update_buffer) {
    board.draw_queue.point.size = 0;
    board.draw_queue.circle.size = 0;
    board.draw_queue.line.size = 0;
    object_traverse(get_draw_buffer);
    board.should_update_buffer = false;
  }

  for (GeomId i = 0; i < board.draw_queue.circle.size; i++) {
    const DrawBuffer buff = board.draw_queue.circle.elems[i];
    rl_draw_circle_lines_v(buff.data.cr.center, buff.data.cr.radius,
                           buff.color);
  }

  for (GeomId i = 0; i < board.draw_queue.line.size; i++) {
    const DrawBuffer buff = board.draw_queue.line.elems[i];
    rl_draw_line_v(buff.data.ln.pt1, buff.data.ln.pt2, buff.color);
  }

  for (GeomId i = 0; i < board.draw_queue.point.size; i++) {
    const DrawBuffer buff = board.draw_queue.point.elems[i];
    rl_draw_circle_v(buff.data.pt, 2, buff.color);
  }
}

static void draw_queue_init(DrawQueue *q, const GeomSize init_size) {
  q->cap = init_size;
  q->size = 0;
  q->elems = malloc(init_size * sizeof(DrawBuffer));
}

static void draw_queue_push(DrawQueue *q, const DrawBuffer elem) {
  if (q->size == q->cap) {
    q->cap *= 2;
    void *mem = realloc(q->elems, q->cap * sizeof(DrawBuffer));
    if (!mem) abort();
    q->elems = mem;
  }
  q->elems[q->size++] = elem;
}

static inline Vec2 transform_point(const float x, const float y) {
  const Vec2 scaled = {x * board.xform_scale, y * board.xform_scale};
  const Vec2 rotated = {
      board.xform_rotate[0][0] * scaled.x + board.xform_rotate[0][1] * scaled.y,
      board.xform_rotate[1][0] * scaled.x + board.xform_rotate[1][1] * scaled.y,
  };
  const Vec2 translated = {rotated.x + board.xform_translate.x,
                           rotated.y + board.xform_translate.y};
  return translated;
}

static inline Color to_raylib_color(const int32_t color) {
  return (Color){color >> 16, color >> 8, color, 255};
}

static void get_draw_buffer(const GeomObject *obj) {
  DrawBuffer buff;
  const GeomId *args = obj->args;
  switch (obj->type) {
  case POINT: {
    const float x = graph_get_value(args[0]);
    const float y = graph_get_value(args[1]);
    buff.data.pt = transform_point(x, y);
    buff.color = obj->color == -1 ? board.default_color.point
                                  : to_raylib_color(obj->color);
    draw_queue_push(&board.draw_queue.point, buff);
    break;
  }
  case CIRCLE: {
    const float cx = graph_get_value(args[0]);
    const float cy = graph_get_value(args[1]);
    buff.data.cr.center = transform_point(cx, cy);
    buff.data.cr.radius = graph_get_value(args[2]) * board.xform_scale;
    buff.color = obj->color == -1 ? board.default_color.circle
                                  : to_raylib_color(obj->color);
    draw_queue_push(&board.draw_queue.circle, buff);
    break;
  }
  default: {
    const float nx = graph_get_value(args[0]);
    const float ny = graph_get_value(args[1]);
    const float dd = graph_get_value(args[2]);
    const float t1 = graph_get_value(args[3]);
    const float t2 = graph_get_value(args[4]);
    buff.data.ln.pt1 = transform_point(nx * dd + ny * t1, ny * dd - nx * t1);
    buff.data.ln.pt2 = transform_point(nx * dd + ny * t2, ny * dd - nx * t2);
    buff.color = obj->color == -1 ? board.default_color.line
                                  : to_raylib_color(obj->color);
    draw_queue_push(&board.draw_queue.line, buff);
  }
  }
}