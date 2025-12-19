#include "object.h"
#include "str_hash.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct {
  size_t capacity, size;
  GeomObject *vector;
  StringHashTable hash_table;
} objects;

static struct {
  struct {
    int point, circle, line;
  } color;
} default_option = {
    {0x000080, 0x10101010, 0x101010}
};

static void get_default_name(char *name) {
  static unsigned int id = 0;
  sprintf(name, "$%05u", id++);
}

void object_init() {
  objects.capacity = 256;
  objects.size = 0;
  objects.vector = malloc(objects.capacity * sizeof(GeomObject));
  string_hash_init(&objects.hash_table, objects.capacity);
}

void object_cleanup() {
  free(objects.vector);
  string_hash_free(&objects.hash_table);
}

ObjectType get_type_from_str(const char *str) {
  uint64_t hash = 0;
  for (int i = 0; *str && i < 8; ++i) {
    hash = (hash << 8) | *str++;
  }
  switch (hash) {
  case 0x706f696e74:
    return POINT;
  case 0x636972636c65:
    return CIRCLE;
  case 0x6c696e65:
    return LINE;
  case 0x726179:
    return RAY;
  case 0x736567:
    return SEG;
  default:
    return UNKNOWN;
  }
}

int get_coord_from_str(const char *str, Point2f *coord) {
  sscanf(str, "%f,%f", &coord->x, &coord->y) == 2;
}

GeomObject *find_object(const ObjectType type, const char *name) {
  const int idx = string_hash_find(&objects.hash_table, name);
  return idx == -1 ? NULL : objects.vector + idx;
}

GeomObject *create_object(const ObjectType type, PointObject *pt1,
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
  if (color == -1) {
    if (type == POINT) obj->color = default_option.color.point;
    else if (type == CIRCLE) obj->color = default_option.color.circle;
    else obj->color = default_option.color.line;
  } else {
    obj->color = color;
  }

  obj->show = show;
  obj->pt1 = pt1;
  obj->pt2 = pt2;
  return obj;
}