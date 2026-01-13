#ifndef GGB_HASH_H
#define GGB_HASH_H

#include <stdint.h>

typedef struct StringHashEntry_ StringHashEntry;

struct StringHashEntry_ {
  int id;
  uint32_t hash;
  const char *str;
  StringHashEntry *next;
};

typedef struct {
  int cap, size;
  StringHashEntry **entries;

  int head;
  int *state;
} StringHashTable;

void string_hash_init(StringHashTable *table, int init_size);
int string_hash_alloc_id(StringHashTable *table);
void string_hash_insert(StringHashTable *table, const char *str, int id);
int string_hash_remove(StringHashTable *table, const char *str);
int string_hash_find(const StringHashTable *table, const char *str);
void string_hash_free(const StringHashTable *table);
// new_size > old_size
void string_hash_resize(StringHashTable *table, int new_size);

static inline const int *string_hash_get_state(const StringHashTable *table) {
  return table->state;
}

#endif //GGB_HASH_H