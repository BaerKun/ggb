#include "raylib.h"
#include "object.h"
#include "str_hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  StringHashTable hash;
  GeomSparseArray array;
} GeomDict;

static GeomDict points, circles, lines;

static inline void geom_dict_init(GeomDict *dict, const int init_size) {
  string_hash_init(&dict->hash, init_size);
  dict->array.cap = init_size;
  dict->array.size = 0;
  dict->array.state = string_hash_get_state(&dict->hash);
  dict->array.data = calloc(init_size, sizeof(GeomObject));
}

static inline void geom_dict_free(const GeomDict *dict) {
  free(dict->array.data);
  string_hash_free(&dict->hash);
}

static void get_default_name(char *name) {
  static unsigned int id = 0;
  sprintf(name, "$%05u", id++);
}

static GeomObject *geom_dict_insert(GeomDict *dict, const char *key) {
  if (dict->array.size++ == dict->array.cap) {
    void *new_buff = realloc(dict->array.data,
                             (dict->array.cap *= 2) * sizeof(GeomObject));
    if (new_buff == NULL) return NULL;
    dict->array.data = new_buff;
    string_hash_resize(&dict->hash, dict->array.cap);
    dict->array.state = string_hash_get_state(&dict->hash);
  }
  const int idx = string_hash_alloc_id(&dict->hash);
  GeomObject *obj = dict->array.data + idx;
  if (key == NULL) get_default_name(obj->name);
  else memcpy(obj->name, key, sizeof(obj->name));
  string_hash_insert(&dict->hash, obj->name, idx);
  return obj;
}

void object_module_init() {
  geom_dict_init(&points, 128);
  geom_dict_init(&circles, 32);
  geom_dict_init(&lines, 64);
  point_module_init(256);
}

void object_module_cleanup() {
  geom_dict_free(&points);
  geom_dict_free(&circles);
  geom_dict_free(&lines);
  point_module_cleanup();
}

GeomObject *object_find(const ObjectType type, const char *name) {
  int idx;
  switch (type) {
  case UNKNOWN:
    return NULL;
  case ANY:
    idx = string_hash_find(&points.hash, name);
    if (idx != -1) return points.array.data + idx;
    idx = string_hash_find(&circles.hash, name);
    if (idx != -1) return circles.array.data + idx;
  default: // LINE, RAY, SEG
    idx = string_hash_find(&lines.hash, name);
    return idx == -1 ? NULL : lines.array.data + idx;
  case POINT:
    idx = string_hash_find(&points.hash, name);
    return idx == -1 ? NULL : points.array.data + idx;
  case CIRCLE:
    idx = string_hash_find(&circles.hash, name);
    return idx == -1 ? NULL : circles.array.data + idx;
  }
}

GeomObject *object_create(const ObjectType type, PointObject *pt1,
                          PointObject *pt2, const char *name, const int color,
                          const int show) {
  GeomObject *obj;
  switch (type) {
  case POINT:
    obj = geom_dict_insert(&points, name);
    break;
  case CIRCLE:
    obj = geom_dict_insert(&circles, name);
    break;
  default:
    obj = geom_dict_insert(&lines, name);
  }
  obj->type = type;
  obj->show = show;
  obj->color = color;
  obj->pt1 = pt1;
  obj->pt2 = pt2;
  return obj;
}

const GeomSparseArray *get_object_array(const ObjectType type) {
  switch (type) {
  case POINT:
    return &points.array;
  case CIRCLE:
    return &circles.array;
  default:
    return &lines.array;
  }
}

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

bool get_coord_from_str(const char *str, Vec2 *coord) {
  return sscanf(str, "%f,%f", &coord->x, &coord->y) == 2;
}