#include "board.h"
#include "object.h"
#include "raylib_.h"
#include <stdlib.h>
#include <string.h>

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
} BoardBuffer;

typedef struct {
  GeomSize cap, size;
  BoardBuffer *elems;
} BoardQueue;

static struct {
  Rectangle window;
  Font font;
  bool should_update_buffer;
  float xform_scale;
  Vec2 xform_translate;
  float xform_rotate[2][2];
  struct {
    Color point, circle, line;
  } default_color;
  struct {
    BoardQueue point, line, circle, text;
  } visual_queue;
} board;

static void board_queue_init(BoardQueue *q, GeomSize init_size);
static void board_queue_push(BoardQueue *q, BoardBuffer elem);
static void get_board_buffer(const GeomObject *obj);

void board_init(const float x, const float y, const float width,
                const float height) {
  board.default_color.point = DARKBLUE;
  board.default_color.circle = GRAY;
  board.default_color.line = GRAY;

  board.window = (Rectangle){x, y, width, height};
  board.font = rl_get_font_default();

  board.xform_scale = 1.f;
  board.xform_translate.y = height;
  board.xform_rotate[0][0] = 1.f;
  board.xform_rotate[1][1] = -1.f;

  board_queue_init(&board.visual_queue.point, 64);
  board_queue_init(&board.visual_queue.circle, 16);
  board_queue_init(&board.visual_queue.line, 32);
  board_queue_init(&board.visual_queue.text, 64);
  object_module_init();
}

void board_cleanup() { object_module_cleanup(); }

void board_update_buffer() { board.should_update_buffer = true; }

void board_draw_update() {
  if (board.should_update_buffer) {
    board.visual_queue.point.size = 0;
    board.visual_queue.circle.size = 0;
    board.visual_queue.line.size = 0;
    board.visual_queue.text.size = 0;
    object_traverse(get_board_buffer);
    board.should_update_buffer = false;
  }

  for (GeomSize i = 0; i < board.visual_queue.circle.size; i++) {
    const BoardBuffer buff = board.visual_queue.circle.elems[i];
    rl_draw_circle_lines_v(buff.data.cr.center, buff.data.cr.radius,
                           buff.color);
  }

  for (GeomSize i = 0; i < board.visual_queue.line.size; i++) {
    const BoardBuffer buff = board.visual_queue.line.elems[i];
    rl_draw_line_v(buff.data.ln.pt1, buff.data.ln.pt2, buff.color);
  }

  for (GeomSize i = 0; i < board.visual_queue.point.size; i++) {
    const BoardBuffer buff = board.visual_queue.point.elems[i];
    rl_draw_circle_v(buff.data.pt, 2, buff.color);
  }

  for (GeomSize i = 0; i < board.visual_queue.text.size; i++) {
    const BoardBuffer buff = board.visual_queue.text.elems[i];
    rl_draw_text_ex(board.font, buff.data.tx.content, buff.data.tx.pos, 20, 1,
                    buff.color);
  }
}

static void board_queue_init(BoardQueue *q, const GeomSize init_size) {
  q->cap = init_size;
  q->size = 0;
  q->elems = malloc(init_size * sizeof(BoardBuffer));
}

static void board_queue_push(BoardQueue *q, const BoardBuffer elem) {
  if (q->size == q->cap) {
    q->cap *= 2;
    void *mem = realloc(q->elems, q->cap * sizeof(BoardBuffer));
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

static void get_board_buffer(const GeomObject *obj) {
  if (!obj->visible) return;
  BoardBuffer geom_buff, text_buff;
  const GeomId *args = obj->args;
  switch (obj->type) {
  case POINT: {
    const float x = graph_get_value(args[0]);
    const float y = graph_get_value(args[1]);
    const Vec2 pt = transform_point(x, y);
    geom_buff.data.pt = pt;
    geom_buff.color = obj->color == -1 ? board.default_color.point
                                       : to_raylib_color(obj->color);
    board_queue_push(&board.visual_queue.point, geom_buff);

    text_buff.data.tx.pos = pt;
    break;
  }
  case CIRCLE: {
    const float cx = graph_get_value(args[0]);
    const float cy = graph_get_value(args[1]);
    const Vec2 center = transform_point(cx, cy);
    const float radius = graph_get_value(args[2]) * board.xform_scale;
    geom_buff.data.cr.center = center;
    geom_buff.data.cr.radius = radius;
    geom_buff.color = obj->color == -1 ? board.default_color.circle
                                       : to_raylib_color(obj->color);
    board_queue_push(&board.visual_queue.circle, geom_buff);

    text_buff.data.tx.pos =
        (Vec2){center.x + radius / 1.414f, center.y + radius / 1.414f};
    break;
  }
  default: {
    const float nx = graph_get_value(args[0]);
    const float ny = graph_get_value(args[1]);
    const float dd = graph_get_value(args[2]);
    const float t1 = graph_get_value(args[3]);
    const float t2 = graph_get_value(args[4]);
    const Vec2 pt1 = transform_point(nx * dd + ny * t1, ny * dd - nx * t1);
    const Vec2 pt2 = transform_point(nx * dd + ny * t2, ny * dd - nx * t2);
    geom_buff.data.ln.pt1 = pt1;
    geom_buff.data.ln.pt2 = pt2;
    geom_buff.color = obj->color == -1 ? board.default_color.line
                                       : to_raylib_color(obj->color);
    board_queue_push(&board.visual_queue.line, geom_buff);

    text_buff.data.tx.pos =
        (Vec2){(pt1.x + pt2.x) / 2.f, (pt1.y + pt2.y) / 2.f};
  }
  }

  if (obj->name[0] == '#') return;
  memcpy(text_buff.data.tx.content, obj->name, 8);
  text_buff.color = geom_buff.color;
  board_queue_push(&board.visual_queue.text, text_buff);
}