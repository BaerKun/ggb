#include "str_hash.h"
#include <stdlib.h>

static inline uint64_t str_hash(const char *str) {
}

void string_hash_table_init(StringHashTable *table, const size_t init_size) {
  table->capacity = init_size;
  table->size = 0;
  table->table = calloc(init_size, sizeof(int64_t));
}

void string_hash_table_free(const StringHashTable *table) {
  free(table->table);
}

int string_hash_table_insert(StringHashTable *table, const char *str) {
}