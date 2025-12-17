#ifndef GGB_HASH_H
#define GGB_HASH_H

#include <stdint.h>

typedef struct {
  size_t capacity, size;
  uint64_t *table;
} StringHashTable;

#define static_string_hash_table(size) {size, 0, (uint64_t[size]){}};
void string_hash_table_init(StringHashTable *table, size_t init_size);
int string_hash_table_insert(StringHashTable *table, const char *str);
int string_hash_table_find(const StringHashTable *table, const char *str);
void string_hash_table_free(const StringHashTable *table);

#endif //GGB_HASH_H