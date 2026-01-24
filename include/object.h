#ifndef OBJECT_H
#define OBJECT_H

#include "point.h"
#include <stdint.h>

typedef enum {
  POINT = 1,
  CIRCLE = 2,
  LINE = 4,
  RAY = 8,
  SEG = 16,
  ANY = 31
} ObjectType;

typedef struct {
  char name[8];
  ObjectType type;
  Color color;
  GeomId pt1, pt2;
} GeomObject;

#define DEFAULT_COLOR (Color){}

int check_name(const char *name);

void object_module_init();
void object_module_cleanup();
void object_draw_all();

void object_delete(const GeomObject *obj);
GeomObject *object_find(ObjectType type, const char *name);
void object_create(ObjectType type, GeomId pt1, GeomId pt2, const char *name,
                   Color color);

void object_traverse(void (*callback)(const GeomObject *));

#endif // OBJECT_H