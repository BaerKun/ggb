#ifndef OBJECT_H
#define OBJECT_H

#include "point.h"
#include <stdbool.h>
#include <stdint.h>

#define OBJECT_NAME_MAX_LEN 15

typedef enum { UNKNOWN = -1, ANY, POINT, CIRCLE, LINE, RAY, SEG } ObjectType;

typedef struct {
  char name[OBJECT_NAME_MAX_LEN + 1];
  ObjectType type;
  int32_t color;
  GeomId pt1, pt2;
} GeomObject;

bool may_be_coord(const char *str);
ObjectType get_type_from_str(const char *str);
bool get_coord_from_str(const char *str, Vec2 *coord);
int check_name(const char *name);

void object_module_init();
void object_module_cleanup();
void object_draw_all();

void object_delete(const GeomObject *obj);
GeomObject *object_find(ObjectType type, const char *name);
void object_create(ObjectType type, GeomId pt1, GeomId pt2, const char *name,
                   int32_t color);

void object_traverse(void (*callback)(const GeomObject *));

#endif // OBJECT_H