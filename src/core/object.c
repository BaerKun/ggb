#include "raylib.h"
#include "object.h"
#include "str_hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct {
  unsigned capacity, size;
  GeomObject *vector;
  StringHashTable hash_table;
} objects;

static struct {
  struct {
    Color point, circle, line;
  } color;
} default_option;

ObjectType get_type_from_str(const char *str) {
  uint64_t hash = 0; // clang-format off
  for (int i = 0; *str && i < 8; ++i) hash = (hash << 8) | *str++;
  switch (hash) {
  case 0x706f696e74: return POINT;
  case 0x636972636c65: return CIRCLE;
  case 0x6c696e65: return LINE;
  case 0x726179: return RAY;
  case 0x736567: return SEG;
  default: return UNKNOWN;
  } // clang-format on
}

int get_coord_from_str(const char *str, Vector2 *coord) {
  return sscanf(str, "%f,%f", &coord->x, &coord->y) == 2;
}

static void get_default_name(char *name) {
  static unsigned int id = 0;
  sprintf(name, "$%05u", id++);
}

void object_module_init() {
  default_option.color.point = DARKBLUE;
  default_option.color.circle = GRAY;
  default_option.color.line = GRAY;

  const unsigned init_size = 256;
  objects.capacity = init_size;
  objects.size = 0;
  objects.vector = malloc(init_size * sizeof(GeomObject));
  string_hash_init(&objects.hash_table, init_size);
  point_module_init(init_size * 2);
}

void object_module_cleanup() {
  free(objects.vector);
  string_hash_free(&objects.hash_table);
  point_module_cleanup();
}

GeomObject *object_find(const ObjectType type, const char *name) {
  const int idx = string_hash_find(&objects.hash_table, name);
  GeomObject *obj = objects.vector + idx;
  return idx == -1 || (type != ANY && obj->type != type) ? NULL : obj;
}

GeomObject *object_create(const ObjectType type, PointObject *pt1,
                          PointObject *pt2, const char *name, const int color,
                          const int show) {
  if (objects.size++ == objects.capacity) {
    void *new_buff = realloc(objects.vector,
                             (objects.capacity *= 2) * sizeof(GeomObject));
    if (new_buff == NULL) return NULL;
    objects.vector = new_buff;
    string_hash_resize(&objects.hash_table, objects.capacity);
  }

  const int id = string_hash_alloc_id(&objects.hash_table);
  GeomObject *obj = objects.vector + id;
  if (name == NULL) get_default_name(obj->name);
  else memcpy(obj->name, name, sizeof(obj->name));
  string_hash_insert(&objects.hash_table, obj->name, id);

  obj->type = type;
  obj->show = show;
  obj->color = color;
  obj->pt1 = pt1;
  obj->pt2 = pt2;
  return obj;
}

static inline Color to_raylib_color(const int color) {
  // little-endian
  return (Color){(unsigned)color >> 16, (unsigned)color >> 8, color, 255};
}

static inline Vector2 get_end_point(const Vector2 p, const Vector2 q) {
  const Vector2 v = Vector2Subtract(q, p);
  const float norm = Vector2Length(v);
  if (norm == 0) return p;
  const float scale = 4096.f / norm;
  return (Vector2){p.x + v.x * scale, p.y + v.y * scale};
}

void object_draw_all() {
#define GET_RAYLIB_COLOR(type_, color_) ((color_) == -1 ? default_option.color.type_ : to_raylib_color(color_))

  int i;
  string_hash_traverse(objects.hash_table, i) {
    const GeomObject *obj = objects.vector + i;
    const Vector2 *p = &obj->pt1->coord;
    const Vector2 *q = &obj->pt2->coord;
    switch (obj->type) {
    case POINT:
      DrawCircleV(*p, 2, GET_RAYLIB_COLOR(point, obj->color));
      break;
    case CIRCLE:
      DrawCircleLinesV(*p, Vector2Distance(*p, *q),
                       GET_RAYLIB_COLOR(circle, obj->color));
      break;
    case LINE:
      DrawLineV(get_end_point(*p, *q), get_end_point(*q, *p),
                GET_RAYLIB_COLOR(line, obj->color));
      break;
    case RAY:
      DrawLineV(*p, get_end_point(*p, *q),
                GET_RAYLIB_COLOR(line, obj->color));
      break;
    case SEG:
      DrawLineV(*p, *q, GET_RAYLIB_COLOR(line, obj->color));
      break;
    default:
      break;
    }
  }
#undef GET_DEFAULT_COLOR
}