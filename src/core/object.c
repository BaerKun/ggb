#include "object.h"
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
static struct {
  StringHashTable hash;
  GeomSparseArray objects;
} internal;

static uint64_t ctz(uint64_t value);
static GeomId object_alloc();
static void object_remove(GeomId id);
static void object_module_resize();

void object_module_init() {
  const GeomSize init_size = 64;

  internal.objects.cap = init_size;
  internal.objects.size = 0;
  internal.objects.bitmap = calloc(init_size / 8, 1);
  internal.objects.data = malloc(init_size * sizeof(GeomObject));

  string_hash_init(&internal.hash, init_size);
  computation_graph_init(init_size * 4);
}

void object_module_cleanup() {
  free(internal.objects.bitmap);
  free(internal.objects.data);
  string_hash_free(&internal.hash);
  computation_graph_cleanup();
}

GeomObject *object_get(const GeomId id) { return internal.objects.data + id; }

GeomId object_create(const ObjectType type, const GeomId *args) {
  if (internal.objects.size == internal.objects.cap) object_module_resize();

  const GeomId id = object_alloc();
  GeomObject *obj = internal.objects.data + id;
  obj->type = type;
  obj->visible = true;
  obj->color = (Color){0, 0, 0, 0};

  for (int i = 0; i < type_argc[type]; i++) {
    obj->args[i] = args[i];
    graph_ref_value(args[i]);
  }
  return id;
}

void object_delete(const GeomId id) {
  const GeomObject *obj = internal.objects.data + id;
  graph_unref_value(type_argc[obj->type], obj->args);
  object_remove(id);
}

void object_delete_all() {
  internal.objects.size = 0;
  memset(internal.objects.bitmap, 0, internal.objects.cap / 8);
  string_hash_clear(&internal.hash);
  computation_graph_clear();
}

void object_traverse(void (*callback)(GeomId id, const GeomObject *)) {
  const GeomSparseArray *array = &internal.objects;
  for (GeomSize i = 0; i < array->cap; i += 64) {
    uint64_t bitmap = array->bitmap[i >> 6];
    while (bitmap) {
      const uint64_t j = ctz(bitmap);
      const GeomId id = (GeomId)(i | j);
      callback(id, array->data + (i | j));
      bitmap &= bitmap - 1;
    }
  }
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

static GeomId object_alloc() {
  const GeomId id = string_hash_alloc_id(&internal.hash);
  GeomObject *obj = internal.objects.data + id;
  get_default_name(obj->name);
  string_hash_insert(&internal.hash, obj->name, id);
  internal.objects.bitmap[id >> 6] |= 1llu << (id & 63);
  internal.objects.size++;
  return id;
}

static void object_remove(const GeomId id) {
  const GeomObject *obj = internal.objects.data + id;
  string_hash_remove(&internal.hash, obj->name);
  internal.objects.bitmap[id >> 6] ^= 1llu << (id & 63);
  internal.objects.size--;
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
}
