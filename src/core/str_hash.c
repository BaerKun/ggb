#include "str_hash.h"
#include <stdlib.h>
#include <string.h>

static inline uint32_t str_hash(const char *str) {
  uint32_t hash = 5381, c;
  while ((c = (uint32_t)*str++)) hash = ((hash << 5) + hash) ^ c;
  return hash;
}

void string_hash_init(StringHashTable *table, const uint32_t init_size) {
  table->cap = init_size;
  table->size = 0;
  table->entries = calloc(init_size, sizeof(StringHashEntry *));

  table->id_head = 0;
  table->next_id = malloc(init_size * sizeof(int));
  for (int i = 0; i < init_size; i++) table->next_id[i] = i + 1;
  // table->next_id[init_size - 1] = -1;
}

void string_hash_free(const StringHashTable *table) {
  for (uint32_t i = 0; i < table->cap; i++) {
    for (StringHashEntry *entry = table->entries[i]; entry;) {
      StringHashEntry *next = entry->next;
      free(entry);
      entry = next;
    }
  }

  free(table->next_id);
  free(table->entries);
}

int string_hash_alloc_id(StringHashTable *table) {
  const int id = table->id_head;
  if (id == table->cap) return -1;
  table->id_head = table->next_id[table->id_head];
  return id;
}

static StringHashEntry **string_hash_find_list(StringHashEntry **ptr,
                                               const uint32_t hash,
                                               const char *str) {
  for (StringHashEntry *entry = *ptr; entry; entry = *ptr) {
    if (entry->hash == hash && strcmp(entry->str, str) == 0) break;
    ptr = &entry->next;
  }
  return ptr;
}

void string_hash_insert(StringHashTable *table, const char *str, const int id) {
  const uint32_t hash = str_hash(str);
  const uint32_t index = hash % table->cap;
  StringHashEntry **ptr = string_hash_find_list(table->entries + index, hash,
                                                str);

  const size_t str_size = strlen(str) + 1;
  StringHashEntry *entry = malloc(sizeof(StringHashEntry) + str_size);
  entry->id = id;
  entry->hash = hash;
  entry->str = str;
  entry->next = NULL;

  *ptr = entry;
  ++table->size;
}

int string_hash_remove(StringHashTable *table, const char *str) {
  const uint32_t hash = str_hash(str);
  const uint32_t index = hash % table->cap;
  StringHashEntry **ptr = string_hash_find_list(table->entries + index, hash,
                                                str);
  StringHashEntry *entry = *ptr;

  if (entry == NULL) return -1;

  *ptr = entry->next;
  table->next_id[entry->id] = table->id_head;
  table->id_head = entry->id;
  free(entry);

  --table->size;
  return table->id_head;
}

int string_hash_find(const StringHashTable *table, const char *str) {
  const uint32_t hash = str_hash(str);
  const uint32_t index = hash % table->cap;
  StringHashEntry **ptr = string_hash_find_list(table->entries + index, hash,
                                                str);
  return (*ptr) ? (*ptr)->id : -1;
}

void string_hash_resize(StringHashTable *table, const uint32_t new_size) {
  const uint32_t old_size = table->cap;
  void *buff = realloc(table->entries, new_size * sizeof(StringHashEntry *));
  if (buff == NULL) return;
  table->cap = new_size;
  table->entries = buff;
  memset(table->entries + old_size, 0,
         (new_size - old_size) * sizeof(StringHashEntry *));

  buff = realloc(table->next_id, new_size * sizeof(int));
  if (buff == NULL) return;
  table->next_id = buff;
  for (int i = (int)old_size; i < new_size; i++) table->next_id[i] = i + 1;

  for (uint32_t idx = 0; idx < old_size; idx++) {
    StringHashEntry *entry, **ptr = table->entries + idx;
    while ((entry = *ptr)) {
      const uint32_t new_idx = entry->hash % new_size;
      if (new_idx == idx) {
        ptr = &entry->next;
      } else {
        *ptr = entry->next;
        entry->next = table->entries[new_idx];
        table->entries[new_idx] = entry;
      }
    }
  }
}