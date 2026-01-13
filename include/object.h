#ifndef OBJECT_H
#define OBJECT_H

#include <stdbool.h>
#include "point.h"

#define OBJECT_NAME_MAX_LEN 15

typedef enum {
  UNKNOWN = -1, ANY, POINT, CIRCLE, LINE, RAY, SEG
} ObjectType;

typedef struct {
  char name[OBJECT_NAME_MAX_LEN + 1];
  ObjectType type;
  int show, color;
  PointObject *pt1, *pt2;
} GeomObject;

typedef struct {
  int cap, size;
  const int *state; // -1 -> used
  GeomObject *data;
} GeomSparseArray;

static inline bool may_be_coord(const char *str) {
  return *str == '-' || *str == '+' || (*str >= '0' && *str <= '9');
}

ObjectType get_type_from_str(const char *str);
bool get_coord_from_str(const char *str, Vec2 *coord);

void object_module_init();
void object_module_cleanup();
void object_draw_all();

// type: ANY / POINT / CIRCLE / (LINE+RAY+SEG)
GeomObject *object_find(ObjectType type, const char *name);
GeomObject *object_create(ObjectType type, PointObject *pt1, PointObject *pt2,
                          const char *name, int color, int show);

// type: POINT / CIRCLE / (LINE+RAY+SEG)
const GeomSparseArray *get_object_array(ObjectType type);

#endif //OBJECT_H