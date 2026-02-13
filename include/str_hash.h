#ifndef GGB_HASH_H
#define GGB_HASH_H

#include "types.h"

typedef struct StringHashEntry_ StringHashEntry;

typedef struct {
  GeomSize cap;
  GeomId free_head;
  GeomId *heads;
  StringHashEntry *entries;
} StringHashTable;

void string_hash_init(StringHashTable *table, GeomSize init_size);
GeomId string_hash_alloc_id(StringHashTable *table);
void string_hash_insert(const StringHashTable *table, const char *str, GeomId id);
void string_hash_remove(StringHashTable *table, const char *str);
void string_hash_clear(StringHashTable *table);
GeomId string_hash_find(const StringHashTable *table, const char *str);
void string_hash_release(const StringHashTable *table);
// new_size > old_size
void string_hash_resize(StringHashTable *table, GeomSize new_size);

#endif //GGB_HASH_H