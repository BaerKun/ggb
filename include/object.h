#ifndef OBJECT_H
#define OBJECT_H

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

static inline int may_be_coord(const char *str) {
  return *str == '-' || *str == '+' || (*str >= '0' && *str <= '9');
}

ObjectType get_type_from_str(const char *str);
int get_coord_from_str(const char *str, Point2f *coord);

void object_module_init(unsigned init_size);
void object_module_cleanup();
void object_draw_all();

GeomObject *object_find(ObjectType type, const char *name);
GeomObject *object_create(ObjectType type, PointObject *pt1, PointObject *pt2,
                          const char *name, int color, int show);

#endif //OBJECT_H