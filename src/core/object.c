#include "object.h"
#include "message.h"
#include "str_hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  GeomSize cap, size;
  uint64_t *bitmap;
  GeomObject *data;
} GeomSparseArray;

typedef struct {
  StringHashTable hash;
  GeomSparseArray array;
} GeomDict;

static GeomDict objects;
static const char *type_str[] = {"point", "circle", "line", "ray", "segment"};

static inline void geom_dict_init(GeomDict *dict, const GeomSize init_size) {
  string_hash_init(&dict->hash, init_size);
  dict->array.cap = init_size;
  dict->array.size = 0;
  dict->array.bitmap = calloc(init_size / 8, 1);
  dict->array.data = malloc(init_size * sizeof(GeomObject));
}

static inline void geom_dict_free(const GeomDict *dict) {
  free(dict->array.bitmap);
  free(dict->array.data);
  string_hash_free(&dict->hash);
}

void object_module_init() {
  geom_dict_init(&objects, 64);
  computation_graph_init(256);
}

void object_module_cleanup() {
  geom_dict_free(&objects);
  computation_graph_cleanup();
}

static void get_default_name(char *name) {
  static unsigned int id = 1;
  sprintf(name, "$%05u", id++);
}

static void geom_dict_resize(GeomDict *dict) {
  GeomSparseArray *array = &dict->array;
  const GeomSize half_cap = array->cap;
  array->cap *= 2;
  string_hash_resize(&dict->hash, array->cap);

  void *mem = realloc(array->bitmap, array->cap / 8);
  if (!mem) abort();
  array->bitmap = mem;
  memset((char *)array->bitmap + half_cap / 8, 0, half_cap / 8);

  mem = realloc(array->data, array->cap * sizeof(GeomObject));
  if (!mem) abort();
  dict->array.data = mem;
}

static GeomObject *geom_dict_insert(GeomDict *dict, const char *key) {
  const GeomId id = string_hash_alloc_id(&dict->hash);
  GeomObject *obj = dict->array.data + id;
  if (key == NULL) {
    get_default_name(obj->name);
  } else {
    memcpy(obj->name, key, sizeof(obj->name));
  }

  string_hash_insert(&dict->hash, obj->name, id);
  dict->array.bitmap[id >> 6] |= 1llu << (id & 63);
  dict->array.size++;
  return obj;
}

static inline uint64_t ctz(const uint64_t value) {
#if defined(__GNUC__) || defined(__clang__)
  const uint64_t res = __builtin_ctzll(value);
#elif defined(_MSC_VER)
  uint64_t res;
  _BitScanForward64(&res, value);
#endif
  return res;
}

void object_create(const ObjectType type, const GeomId *args,
                   const char *name, const Color color) {
  if (objects.array.size == objects.array.cap) geom_dict_resize(&objects);
  GeomObject *obj = geom_dict_insert(&objects, name);
  obj->type = type;
  obj->color = color;

  static const int argc[] = {2, 3, 5};
  for (int i = 0; i < argc[ctz(type)]; i++) {
    obj->args[i] = args[i];
    graph_ref_value(args[i]);
  }
}


static void object_not_exists(const ObjectType type, const char *name) {
  if (type == ANY) {
    push_error_fmt("object '%s' doesn't exist.", name);
  } else {
    push_error_fmt("%s '%s' doesn't exist.", type_str[ctz(type)], name);
  }
}

static void object_error_type(const ObjectType target, const ObjectType got,
                              const char *name) {
  push_error_fmt("'%s' is a %s, but need %s", name, type_str[ctz(got)],
                 type_str[ctz(target)]);
}

int object_get_args(const ObjectType types, const char *name, GeomId *args) {
  const GeomInt id = string_hash_find(&objects.hash, name);
  if (id == -1) {
    object_not_exists(types, name);
    return MSG_ERROR;
  }

  const GeomObject *obj = objects.array.data + id;
  if (!(obj->type & types)) {
    object_error_type(types, obj->type, name);
    return MSG_ERROR;
  }

  static const int argc[] = {2, 3, 4};
  memcpy(args, obj->args, sizeof(GeomId) * argc[ctz(obj->type)]);
  return 0;
}

int object_delete(const char *name) {
  const GeomId id = string_hash_remove(&objects.hash, name);
  if (id == -1) {
    object_not_exists(ANY, name);
    return MSG_ERROR;
  }

  static const int argc[] = {2, 3, 5};
  const GeomObject *obj = objects.array.data + id;
  for (int i = 0; i < argc[ctz(obj->type)]; i++) {
    graph_unref_value(obj->args[i]);
  }

  objects.array.bitmap[id >> 6] ^= 1llu << (id & 63);
  objects.array.size--;
  return 0;
}

static void geom_dict_clear(GeomDict *dict) {
  GeomSparseArray *array = &dict->array;
  array->size = 0;
  memset(array->bitmap, 0, array->cap / 8);
  string_hash_clear(&dict->hash);
}

void object_delete_all() {
  geom_dict_clear(&objects);
  computation_graph_clear();
}

void object_traverse(void (*callback)(const GeomObject *)) {
  const GeomSparseArray *array = &objects.array;
  for (GeomSize i = 0; i < array->cap; i += 64) {
    uint64_t bitmap = array->bitmap[i >> 6];
    while (bitmap) {
      const uint64_t j = ctz(bitmap);
      callback(array->data + (i | j));
      bitmap &= bitmap - 1;
    }
  }
}

int check_name(const char *name) {
  if (name != NULL) {
    if (strlen(name) > 7) {
      throw_error_fmt("name '%s' is too long. ( <= %d )", name, 7);
    }
    if (string_hash_find(&objects.hash, name) != -1) {
      throw_error_fmt("name '%s' already exists.", name);
    }
  }
  return 0;
}

int parse_color(const char *str, Color *color) {
  if (str == NULL) {
    *color = (Color){};
    return 0;
  }

  if (strlen(str) <= 6) {
    char *end;
    const uint32_t value = strtoul(str, &end, 16);
    if (*end == 0) {
      *color = (Color){value >> 16, value >> 8, value, 255};
      return 0;
    }
  }

  throw_error_fmt("color should be a hex-number like 'RRGGBB'. got '%s'.", str);
}