#include "str_hash.h"
#include <stdlib.h>
#include <string.h>

struct StringHashEntry_ {
  GeomId next;
  uint64_t hash;
  const char *str;
};

static inline uint64_t str_hash(const char *str) {
  uint64_t hash = 5381, c;
  while ((c = (uint64_t)*str++)) hash = ((hash << 5) + hash) ^ c;
  return hash;
}

void string_hash_init(StringHashTable *table, const GeomSize init_size) {
  table->cap = init_size;
  table->free_head = 0;
  table->heads = malloc(init_size * sizeof(GeomId));
  table->entries = malloc(init_size * sizeof(StringHashEntry));

  memset(table->heads, -1, init_size * sizeof(GeomId));
  for (GeomId i = 0; i < init_size; i++) table->entries[i].next = i + 1;
}

void string_hash_free(const StringHashTable *table) {
  free(table->heads);
  free(table->entries);
}

GeomId string_hash_alloc_id(StringHashTable *table) {
  const GeomId id = table->free_head;
  // if (id == table->cap) return -1;
  table->free_head = table->entries[id].next;
  return id;
}

void string_hash_insert(const StringHashTable *table, const char *str,
                        const GeomId id) {
  StringHashEntry *entry = table->entries + id;
  entry->hash = str_hash(str);
  entry->str = str;

  GeomId *head = table->heads + entry->hash % table->cap;
  entry->next = *head;
  *head = id;
}

GeomId string_hash_remove(StringHashTable *table, const char *str) {
  const uint64_t hash = str_hash(str);
  GeomId *ptr = table->heads + hash % table->cap;
  for (GeomId i; (i = *ptr) != -1;) {
    StringHashEntry *entry = table->entries + i;
    if (entry->hash == hash && strcmp(entry->str, str) == 0) {
      *ptr = entry->next;
      entry->next = table->free_head;
      table->free_head = i;
      return i;
    }
    ptr = &entry->next;
  }
  return -1;
}

GeomId string_hash_find(const StringHashTable *table, const char *str) {
  const uint64_t hash = str_hash(str);
  for (GeomId i = table->heads[hash % table->cap]; i != -1;) {
    const StringHashEntry *entry = table->entries + i;
    if (entry->hash == hash && strcmp(entry->str, str) == 0) return i;
    i = entry->next;
  }
  return -1;
}

void string_hash_resize(StringHashTable *table, const GeomSize new_size) {
  const GeomSize old_size = table->cap;
  table->cap = new_size;

  void *new_memory =
      realloc(table->entries, new_size * sizeof(StringHashEntry));
  table->entries = new_memory;
  for (GeomId i = (GeomId)old_size; i < new_size; i++) {
    table->entries[i].next = i + 1;
  }

  new_memory = realloc(table->heads, new_size * sizeof(int));
  table->heads = new_memory;
  memset(table->heads, -1, new_size * sizeof(int));
  for (GeomId i = 0; i < old_size; i++) {
    StringHashEntry *entry = table->entries + i;
    GeomId *new_head = table->heads + entry->hash % new_size;
    entry->next = *new_head;
    *new_head = i;
  }
}