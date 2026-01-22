#include "object.h"
#include "str_hash.h"
#include "message.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int cap, size;
  uint64_t *bitmap;
  GeomObject *data;
} GeomSparseArray;

typedef struct {
  StringHashTable hash;
  GeomSparseArray array;
} GeomDict;

static GeomDict objects;

static inline void geom_dict_init(GeomDict *dict, const int init_size) {
  string_hash_init(&dict->hash, init_size);
  dict->array.cap = init_size;
  dict->array.size = 0;
  dict->array.bitmap = calloc(init_size / 8, 1);
  dict->array.data = calloc(init_size, sizeof(GeomObject));
}

static inline void geom_dict_free(const GeomDict *dict) {
  free(dict->array.bitmap);
  free(dict->array.data);
  string_hash_free(&dict->hash);
}

static void get_default_name(char *name) {
  static unsigned int id = 1;
  sprintf(name, "$%05u", id++);
}

static GeomObject *geom_dict_insert(GeomDict *dict, const char *key) {
  GeomSparseArray *array = &dict->array;
  if (array->size == array->cap) {
    array->cap *= 2;
    string_hash_resize(&dict->hash, array->cap);

    void *new_memory = realloc(array->bitmap, array->cap / 8);
    array->bitmap = new_memory;
    memset(array->bitmap + array->size / 8, 0, array->cap / 16);

    new_memory = realloc(array->data, array->cap * sizeof(GeomObject));
    dict->array.data = new_memory;
  }

  const int idx = string_hash_alloc_id(&dict->hash);
  GeomObject *obj = array->data + idx;
  if (key == NULL) {
    get_default_name(obj->name);
  } else {
    memcpy(obj->name, key, sizeof(obj->name));
  }
  string_hash_insert(&dict->hash, obj->name, idx);

  array->bitmap[idx >> 6] |= 1llu << (idx & 63);
  array->size++;
  return obj;
}

static void geom_dict_delete(GeomDict *dict, const char *key) {
  const GeomId id = string_hash_remove(&dict->hash, key);
  dict->array.bitmap[id >> 6] ^= 1llu << (id & 63);
  dict->array.size--;
}

void object_module_init() {
  geom_dict_init(&objects, 64);
  point_module_init(128);
}

void object_module_cleanup() {
  geom_dict_free(&objects);
  point_module_cleanup();
}

GeomObject *object_find(const ObjectType type, const char *name) {
  const GeomInt id = string_hash_find(&objects.hash, name);
  GeomObject *obj = objects.array.data + id;
  if (id == -1 || (type != ANY && type != obj->type)) return NULL;
  return obj;
}

void object_create(const ObjectType type, const GeomId pt1, const GeomId pt2,
                   const char *name, const int32_t color, const bool show) {
  GeomObject *obj = geom_dict_insert(&objects, name);
  obj->type = type;
  obj->show = show;
  obj->color = color;
  obj->pt1 = pt1;
  obj->pt2 = pt2;
  point_ref(pt1);
  point_ref(pt2);
}

void object_delete(const GeomObject *obj) {
  point_unref(obj->pt1);
  point_unref(obj->pt2);
  geom_dict_delete(&objects, obj->name);
}

void object_traverse(void (*callback)(const GeomObject *)) {
  const GeomSparseArray *array = &objects.array;
  for (int i = 0; i < array->cap; i += 64) {
    uint64_t bitmap = array->bitmap[i >> 6];
    while (bitmap) {
#if defined(__GNUC__) || defined(__clang__)
      const uint64_t j = __builtin_ctzll(bitmap);
#elif defined(_MSC_VER)
      uint64_t j;
      _BitScanForward64(&j, bitmap);
#endif
      callback(array->data + (i | j));
      bitmap &= bitmap - 1;
    }
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

bool may_be_coord(const char *str) {
  return *str == '-' || *str == '+' || (*str >= '0' && *str <= '9');
}

bool get_coord_from_str(const char *str, Vec2 *coord) {
  return sscanf(str, "%f,%f", &coord->x, &coord->y) == 2;
}

int check_name(const char *name) {
  if (name != NULL) {
    if (strlen(name) > OBJECT_NAME_MAX_LEN) {
      throw_error_fmt("name '%s' is too long. ( <= %d )", name,
                      OBJECT_NAME_MAX_LEN);
    }
    if (object_find(ANY, name) != NULL) {
      throw_error_fmt("name '%s' already exists.", name);
    }
  }
  return 0;
}