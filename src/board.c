#include "board.h"
#include "object.h"
#include "raylib_.h"
#include "types.h"
#include <math.h>
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

struct {
  DrawQueue point, line, circle;
} draw_buffer;

static struct {
  struct {
    Color point, circle, line;
  } color;
} default_config;

static bool should_update_buffer;

static void draw_queue_init(DrawQueue *q, GeomSize init_size);
static void draw_queue_push(DrawQueue *q, DrawBuffer elem);
static void get_draw_buffer(const GeomObject *obj);

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

void board_update_buffer() { should_update_buffer = true; }

void board_draw_update() {
  if (should_update_buffer) {
    draw_buffer.point.size = draw_buffer.circle.size = draw_buffer.line.size =
        0;
    object_traverse(get_draw_buffer);
    should_update_buffer = false;
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

  for (GeomId i = 0; i < draw_buffer.point.size; i++) {
    const DrawBuffer buff = draw_buffer.point.elems[i];
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
    buff.data.pt = (Vec2){x, y};
    buff.color = obj->color == -1 ? default_config.color.point
                                  : to_raylib_color(obj->color);
    draw_queue_push(&draw_buffer.point, buff);
    break;
  }
  case CIRCLE: {
    const float cx = graph_get_value(args[0]);
    const float cy = graph_get_value(args[1]);
    buff.data.cr.center = (Vec2){cx, cy};
    buff.data.cr.radius = graph_get_value(args[2]);
    buff.color = obj->color == -1 ? default_config.color.circle
                                  : to_raylib_color(obj->color);
    draw_queue_push(&draw_buffer.circle, buff);
    break;
  }
  default: {
    const float nx = graph_get_value(args[0]);
    const float ny = graph_get_value(args[1]);
    const float dd = graph_get_value(args[2]);
    const float t1 = graph_get_value(args[3]);
    const float t2 = graph_get_value(args[4]);
    buff.data.ln.pt1 = (Vec2){dd * nx + t1 * ny, dd * ny - t1 * nx};
    buff.data.ln.pt2 = (Vec2){dd * nx + t2 * ny, dd * ny - t2 * nx};
    buff.color = obj->color == -1 ? default_config.color.line
                                  : to_raylib_color(obj->color);
    draw_queue_push(&draw_buffer.line, buff);
  }
  }
}