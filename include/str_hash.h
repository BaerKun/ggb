#ifndef GGB_HASH_H
#define GGB_HASH_H

#include <stdint.h>

typedef struct StringHashEntry_ StringHashEntry;
typedef struct {
  uint32_t cap, size;
  StringHashEntry **entries;

  int id_head;
  int *next_id;
} StringHashTable;

void string_hash_init(StringHashTable *table, uint32_t init_size);
int string_hash_insert(StringHashTable *table, const char *str);
int string_hash_remove(StringHashTable *table, const char *str);
int string_hash_find(const StringHashTable *table, const char *str);
void string_hash_free(const StringHashTable *table);

#endif //GGB_HASH_H