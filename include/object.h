#ifndef OBJECT_H
#define OBJECT_H

#include "graph.h"

typedef enum {
  UNKNOWN = 0,
  POINT = 1,
  CIRCLE = 2,
  LINE = 4,
  ANY = 7
} ObjectType;

typedef struct {
  char name[8];
  ObjectType type;
  bool visible;
  int32_t color;
  GeomId group;
  GeomId args[5];
} GeomObject;

int parse_new_name(char *str, GeomSize count, char **names);
int parse_color(const char *str, int32_t *color);
int check_group(GeomId group);

void object_module_init();
void object_module_cleanup();

int object_delete(const char *name);
void object_delete_all();
ObjectType object_get_args(ObjectType types, const char *name, GeomId *args);
void object_create(ObjectType type, const GeomId *args, const char *name,
                   GeomId group, int32_t color);

void object_traverse(void (*callback)(const GeomObject *));

#endif // OBJECT_H