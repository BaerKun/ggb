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

static const GeomSize type_argc[] = {0, 2, 3, 0, 5};
static const char *type_str[] = {NULL, "point", "circle", NULL, "line"};
static struct {
  StringHashTable hash;
  GeomSparseArray objects;
  GeomId group_heads[17];
  GeomId *group_next;
} internal;

static uint64_t ctz(uint64_t value);
static GeomObject *object_insert(const char *name, GeomId group);
static GeomObject *object_remove(const char *name);
static void object_module_resize();
static void object_not_exists(ObjectType type, const char *name);
static void object_error_type(ObjectType target, ObjectType got,
                              const char *name);

void object_module_init() {
  const GeomSize init_size = 64;

  internal.objects.cap = init_size;
  internal.objects.size = 0;
  internal.objects.bitmap = calloc(init_size / 8, 1);
  internal.objects.data = malloc(init_size * sizeof(GeomObject));

  internal.group_next = malloc(init_size * sizeof(GeomId));
  memset(internal.group_heads, -1, sizeof(internal.group_heads));

  string_hash_init(&internal.hash, init_size);
  computation_graph_init(init_size * 4);
}

void object_module_cleanup() {
  free(internal.objects.bitmap);
  free(internal.objects.data);
  free(internal.group_next);
  string_hash_free(&internal.hash);
  computation_graph_cleanup();
}

void object_create(const ObjectType type, const GeomId *args, const char *name,
                   const GeomId group, const int32_t color) {
  if (internal.objects.size == internal.objects.cap) object_module_resize();

  GeomObject *obj = object_insert(name, group);
  obj->type = type;
  obj->visible = true;
  obj->color = color;

  for (int i = 0; i < type_argc[type]; i++) {
    obj->args[i] = args[i];
    graph_ref_value(args[i]);
  }
}

int object_delete(const char *name) {
  const GeomObject *obj = object_remove(name);
  if (obj == NULL) {
    object_not_exists(ANY, name);
    return MSG_ERROR;
  }

  graph_unref_value(type_argc[obj->type], obj->args);
  return 0;
}

void object_delete_all() {
  internal.objects.size = 0;
  memset(internal.objects.bitmap, 0, internal.objects.cap / 8);
  memset(internal.group_heads, -1, sizeof(internal.group_heads));
  string_hash_clear(&internal.hash);
  computation_graph_clear();
}

void object_traverse(void (*callback)(const GeomObject *)) {
  const GeomSparseArray *array = &internal.objects;
  for (GeomSize i = 0; i < array->cap; i += 64) {
    uint64_t bitmap = array->bitmap[i >> 6];
    while (bitmap) {
      const uint64_t j = ctz(bitmap);
      callback(array->data + (i | j));
      bitmap &= bitmap - 1;
    }
  }
}

ObjectType object_get_args(const ObjectType types, const char *name,
                           GeomId *args) {
  const GeomInt id = string_hash_find(&internal.hash, name);
  if (id == -1) {
    object_not_exists(types, name);
    return UNKNOWN;
  }

  const GeomObject *obj = internal.objects.data + id;
  if (!(obj->type & types)) {
    object_error_type(types, obj->type, name);
    return UNKNOWN;
  }

  memcpy(args, obj->args, sizeof(GeomId) * type_argc[obj->type]);
  return obj->type;
}

int parse_new_name(char *str, const GeomSize count, char **names) {
  memset(names, 0, count * sizeof(void *));
  if (str == NULL) return 0;

  for (GeomSize i = 0; i < count; i++) {
    if (*str == 0) break;
    names[i] = str;
    while (*str && *str != ',') str++;
    if (*str) *str++ = 0;
  }

  if (*str) {
    message_push_back(MSG_WARN, "only need %u names.", count);
  }

  for (GeomSize i = 0; i < count; i++) {
    const char *name = names[i];
    if (name == NULL) continue;

    if (strlen(name) > 7) {
      throw_error_fmt("name '%s' is too long. ( <= %d )", name, 7);
    }

    if (string_hash_find(&internal.hash, name) != -1) {
      throw_error_fmt("name '%s' already exists.", name);
    }
  }
  return 0;
}

int parse_color(const char *str, int32_t *color) {
  if (str == NULL) {
    *color = -1;
    return 0;
  }
  if (strlen(str) <= 6) {
    char *end;
    const uint32_t value = strtoul(str, &end, 16);
    if (*end == 0) {
      *color = (int32_t)value;
      return 0;
    }
  }
  throw_error_fmt("color should be a hex-number like 'RRGGBB'. got '%s'.", str);
}

int check_group(const GeomId group) {
  if (group < 0 || group > 15) {
    throw_error("group should between [0, 15].");
  }
  return 0;
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

static void get_default_name(char *name) {
  static unsigned int id = 1;
  sprintf(name, "#%05u", id++);
}

static GeomObject *object_insert(const char *name, const GeomId group) {
  const GeomId id = string_hash_alloc_id(&internal.hash);
  GeomObject *obj = internal.objects.data + id;
  name ? memcpy(obj->name, name, sizeof(obj->name))
       : get_default_name(obj->name);

  string_hash_insert(&internal.hash, obj->name, id);
  internal.objects.bitmap[id >> 6] |= 1llu << (id & 63);
  internal.objects.size++;

  obj->group = group;
  internal.group_next[id] = internal.group_heads[group];
  internal.group_heads[group] = id;
  return obj;
}

static GeomObject *object_remove(const char *name) {
  const GeomId id = string_hash_remove(&internal.hash, name);
  if (id == -1) return NULL;

  GeomObject *obj = internal.objects.data + id;
  GeomId *ptr = internal.group_heads + obj->group;
  for (; *ptr != id; ptr = internal.group_next + *ptr);
  *ptr = internal.group_next[id];

  internal.objects.bitmap[id >> 6] ^= 1llu << (id & 63);
  internal.objects.size--;
  return obj;
}

static void object_module_resize() {
  GeomSparseArray *objects = &internal.objects;
  const GeomSize half_cap = objects->cap;

  objects->cap *= 2;
  string_hash_resize(&internal.hash, objects->cap);

  void *mem = realloc(objects->bitmap, objects->cap / 8);
  if (!mem) abort();
  objects->bitmap = mem;
  memset((char *)objects->bitmap + half_cap / 8, 0, half_cap / 8);

  mem = realloc(objects->data, objects->cap * sizeof(GeomObject));
  if (!mem) abort();
  objects->data = mem;

  mem = realloc(internal.group_next, objects->cap * sizeof(GeomId));
  if (!mem) abort();
  internal.group_next = mem;
}

static void object_not_exists(const ObjectType type, const char *name) {
  if (type == ANY) {
    push_error_fmt("object '%s' doesn't exist.", name);
  } else {
    push_error_fmt("%s '%s' doesn't exist.", type_str[type], name);
  }
}

static void object_error_type(const ObjectType target, const ObjectType got,
                              const char *name) {
  push_error_fmt("'%s' is a %s, but need %s", name, type_str[got],
                 type_str[target]);
}
