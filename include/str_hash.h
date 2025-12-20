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
  uint32_t cap, size;
  StringHashEntry **entries;

  int id_head;
  int *next_id;
} StringHashTable;

#define string_hash_traverse(hash, id) \
  for (uint32_t i__ = 0; i__ < (hash).cap; i__++) \
    for (StringHashEntry *entry__ = (hash).entries[i__]; \
        ((id) = entry__->id, entry__); entry__ = entry__->next)

void string_hash_init(StringHashTable *table, uint32_t init_size);
int string_hash_alloc_id(StringHashTable *table);
void string_hash_insert(StringHashTable *table, const char *str, int id);
int string_hash_remove(StringHashTable *table, const char *str);
int string_hash_find(const StringHashTable *table, const char *str);
void string_hash_free(const StringHashTable *table);
// new_size > old_size
void string_hash_resize(StringHashTable *table, uint32_t new_size);

#endif //GGB_HASH_H