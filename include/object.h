#ifndef OBJECT_H
#define OBJECT_H

#include "graph.h"

typedef enum { POINT = 1, CIRCLE = 2, LINE = 4, ANY = 7 } ObjectType;

typedef struct {
  char name[8];
  ObjectType type;
  Color color;
  GeomId args[5];
} GeomObject;

int check_name(const char *name);
int parse_color(const char *str, Color *color);

void object_module_init();
void object_module_cleanup();
void object_draw_all();

int object_delete(const char *name);
void object_delete_all();
int object_get_args(ObjectType types, const char *name, GeomId *args);
void object_create(ObjectType type, const GeomId *args, const char *name,
                   Color color);

void object_traverse(void (*callback)(const GeomObject *));

#endif // OBJECT_H