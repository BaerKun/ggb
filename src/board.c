#include "board.h"
#include "console.h"
#include "object.h"
#include "raylib_.h"
#include <math.h>
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
  } data;
  char name[8];
  Vec2 name_pos;
  Color color;
} BoardGeomObject;

typedef struct {
  GeomSize cap, size;
  BoardGeomObject *elems;
} BoardGeomQueue;

static struct {
  Rectangle window;
  Font font;
  bool should_update_buffer;

  float xform_scale;
  Vec2 xform_translate;
  float xform_rotate[2][2];

  char selected_object[8];

  struct {
    Color point, circle, line;
  } default_color;
  struct {
    BoardGeomQueue point, line, circle;
  } visual_queue;
} board;

#define for_in_queue(ptr, queue)                                               \
  for (GeomSize i = 0; i < queue.size; i++)                                    \
    if (ptr = queue.elems + i, 1)

static void board_queue_init(BoardGeomQueue *q, GeomSize init_size);
static BoardGeomObject *board_queue_alloc(BoardGeomQueue *q);
static bool check_collision_point_point(Vec2 pt1, Vec2 pt2, float radius);
static const char *mouse_select_object(Vec2 pos);
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
  object_module_init();
}

void board_cleanup() {
  free(board.visual_queue.point.elems);
  free(board.visual_queue.line.elems);
  free(board.visual_queue.circle.elems);
  object_module_cleanup();
}

void board_listen() {
  static uint64_t used_bitmap = 0;
  static enum { NONE, LEFT, RIGHT } pressed_button = NONE;
  static Vec2 pressed_pos = {0, 0};

  const Vec2 mouse_pos = rl_get_mouse_position();
  if (pressed_button == NONE &&
      rl_check_collision_point_rec(mouse_pos, board.window)) {
    if (rl_is_mouse_button_pressed(MOUSE_BUTTON_LEFT)) {
      pressed_button = LEFT;
      pressed_pos = mouse_pos;
    } else if (rl_is_mouse_button_pressed(MOUSE_BUTTON_RIGHT)) {
      pressed_button = RIGHT;
      pressed_pos = mouse_pos;
    }
  }

  if (pressed_button == LEFT &&
      rl_is_mouse_button_released(MOUSE_BUTTON_LEFT)) {
    if (check_collision_point_point(mouse_pos, pressed_pos, 3)) {
      const char *obj_name = mouse_select_object(mouse_pos);
      if (obj_name) {
        console_push_text(obj_name, true);
        strcpy(board.selected_object, obj_name);
      }
    }
    pressed_button = NONE;
    pressed_pos = (Vec2){0, 0};
  } else if (pressed_button == RIGHT &&
             rl_is_mouse_button_released(MOUSE_BUTTON_RIGHT)) {
    if (check_collision_point_point(mouse_pos, pressed_pos, 3)) {
    }
    pressed_button = NONE;
    pressed_pos = (Vec2){0, 0};
  }
}

void board_draw() {
  if (board.should_update_buffer) {
    board.visual_queue.point.size = 0;
    board.visual_queue.circle.size = 0;
    board.visual_queue.line.size = 0;
    object_traverse(get_board_buffer);
    board.should_update_buffer = false;
  }

  BoardGeomObject *obj;
  for_in_queue(obj, board.visual_queue.circle) {
    rl_draw_circle_lines_v(obj->data.cr.center, obj->data.cr.radius,
                           obj->color);
  }
  for_in_queue(obj, board.visual_queue.line) {
    rl_draw_line_v(obj->data.ln.pt1, obj->data.ln.pt2, obj->color);
  }
  for_in_queue(obj, board.visual_queue.point) {
    rl_draw_circle_v(obj->data.pt, 2, obj->color);
  }
  for_in_queue(obj, board.visual_queue.circle) {
    rl_draw_text_ex(board.font, obj->name, obj->name_pos, 20, 1, obj->color);
  }
  for_in_queue(obj, board.visual_queue.line) {
    rl_draw_text_ex(board.font, obj->name, obj->name_pos, 20, 1, obj->color);
  }
  for_in_queue(obj, board.visual_queue.point) {
    rl_draw_text_ex(board.font, obj->name, obj->name_pos, 20, 1, obj->color);
  }
}

void board_update_buffer() { board.should_update_buffer = true; }

static void board_queue_init(BoardGeomQueue *q, const GeomSize init_size) {
  q->cap = init_size;
  q->size = 0;
  q->elems = malloc(init_size * sizeof(BoardGeomObject));
}

static BoardGeomObject *board_queue_alloc(BoardGeomQueue *q) {
  if (q->size == q->cap) {
    q->cap *= 2;
    void *mem = realloc(q->elems, q->cap * sizeof(BoardGeomObject));
    if (!mem) abort();
    q->elems = mem;
  }
  return q->elems + q->size++;
}

static bool check_collision_point_point(const Vec2 pt1, const Vec2 pt2,
                                        const float radius) {
  return fabsf(pt1.x - pt2.x) <= radius && fabsf(pt1.y - pt2.y) <= radius;
}

static float vec2_distance(const Vec2 v1, const Vec2 v2) {
  const float dx = v1.x - v2.x;
  const float dy = v1.y - v2.y;
  return sqrtf(dx * dx + dy * dy);
}

static const char *mouse_select_object(const Vec2 pos) {
  BoardGeomObject *obj;
  for_in_queue(obj, board.visual_queue.point) {
    if (check_collision_point_point(pos, obj->data.pt, 5)) {
      return obj->name;
    }
  }
  for_in_queue(obj, board.visual_queue.line) {
    if (rl_check_collision_point_line(pos, obj->data.ln.pt1, obj->data.ln.pt2,
                                      5)) {
      return obj->name;
    }
  }
  for_in_queue(obj, board.visual_queue.circle) {
    const float dist = vec2_distance(pos, obj->data.cr.center);
    if (fabsf(dist - obj->data.cr.radius) <= 5) {
      return obj->name;
    }
  }
  return NULL;
}

static Vec2 transform_point(const float x, const float y) {
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
  const GeomId *args = obj->args;
  switch (obj->type) {
  case POINT: {
    const float x = graph_get_value(args[0]);
    const float y = graph_get_value(args[1]);
    const Vec2 pt = transform_point(x, y);

    BoardGeomObject *b_obj = board_queue_alloc(&board.visual_queue.point);
    memcpy(b_obj->name, obj->name, sizeof(b_obj->name));
    b_obj->data.pt = pt;
    b_obj->name_pos = pt;
    b_obj->color = obj->color == -1 ? board.default_color.point
                                    : to_raylib_color(obj->color);
    break;
  }
  case CIRCLE: {
    const float cx = graph_get_value(args[0]);
    const float cy = graph_get_value(args[1]);
    const Vec2 center = transform_point(cx, cy);
    const float radius = graph_get_value(args[2]) * board.xform_scale;

    BoardGeomObject *b_obj = board_queue_alloc(&board.visual_queue.circle);
    memcpy(b_obj->name, obj->name, sizeof(b_obj->name));
    b_obj->data.cr.center = center;
    b_obj->data.cr.radius = radius;
    b_obj->name_pos =
        (Vec2){center.x + radius / 1.414f, center.y + radius / 1.414f};
    b_obj->color = obj->color == -1 ? board.default_color.circle
                                    : to_raylib_color(obj->color);
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

    BoardGeomObject *b_obj = board_queue_alloc(&board.visual_queue.line);
    memcpy(b_obj->name, obj->name, sizeof(b_obj->name));
    b_obj->data.ln.pt1 = pt1;
    b_obj->data.ln.pt2 = pt2;
    b_obj->name_pos = (Vec2){(pt1.x + pt2.x) / 2.f, (pt1.y + pt2.y) / 2.f};
    b_obj->color = obj->color == -1 ? board.default_color.line
                                    : to_raylib_color(obj->color);
  }
  }
}