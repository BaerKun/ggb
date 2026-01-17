#ifndef GGB_HASH_H
#define GGB_HASH_H

typedef struct StringHashEntry_ StringHashEntry;

typedef struct {
  int cap;
  int free_head;
  int *heads;
  StringHashEntry *entries;
} StringHashTable;

void string_hash_init(StringHashTable *table, int init_size);
int string_hash_alloc_id(StringHashTable *table);
void string_hash_insert(const StringHashTable *table, const char *str, int id);
int string_hash_remove(StringHashTable *table, const char *str);
int string_hash_find(const StringHashTable *table, const char *str);
void string_hash_free(const StringHashTable *table);
// new_size > old_size
void string_hash_resize(StringHashTable *table, int new_size);

#endif //GGB_HASH_H