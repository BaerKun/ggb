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
  bool show;
  GeomId pt1, pt2;
} GeomObject;

typedef struct GeomSparseArray_ GeomSparseArray;

bool may_be_coord(const char *str);
ObjectType get_type_from_str(const char *str);
bool get_coord_from_str(const char *str, Vec2 *coord);

void object_module_init();
void object_module_cleanup();
void object_draw_all();

// type: ANY / POINT / CIRCLE / (LINE+RAY+SEG)
GeomObject *object_find(ObjectType type, const char *name);
GeomObject *object_create(ObjectType type, GeomId pt1, GeomId pt2,
                          const char *name, int32_t color, bool show);

// type: POINT / CIRCLE / (LINE+RAY+SEG)
const GeomSparseArray *get_object_array(ObjectType type);
void object_array_traverse(const GeomSparseArray *array,
                           void (*callback)(const GeomObject *));

#endif // OBJECT_H