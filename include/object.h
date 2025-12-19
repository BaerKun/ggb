#ifndef OBJECT_H
#define OBJECT_H

#include "geometry.h"

#define OBJECT_NAME_MAX_LEN 15

typedef enum {
  UNKNOWN = -1, ANY, POINT, CIRCLE, LINE, RAY, SEG
} ObjectType;

typedef struct PointObject_ PointObject;
typedef struct AdjacencyList_ AdjacencyList;

struct AdjacencyList_ {
  PointObject *pt;
  AdjacencyList *next;
};

struct PointObject_ {
  Point2f coord;
  AdjacencyList *successors;

  int indegree;
  Point2f (*constraint)(int, const PointObject **);
  const PointObject *predecessors[];
};

typedef struct {
  int argc;
  PointObject **argv;
  Point2f (*callback)(int, const PointObject **);
} Constraint;

typedef struct {
  char name[OBJECT_NAME_MAX_LEN + 1];
  ObjectType type;
  int show, color;
  PointObject *pt1, *pt2;
} GeomObject;

void object_init();
void object_cleanup();

static inline int may_be_coord(const char *str) {
  return *str == '-' || *str == '+' || (*str >= '0' && *str <= '9');
}

ObjectType get_type_from_str(const char *str);
int get_coord_from_str(const char *str, Point2f *coord);

PointObject *create_point(Point2f coord, Constraint cons);
void delete_point(PointObject *pt);

GeomObject *create_object(ObjectType type, PointObject *pt1, PointObject *pt2,
                          const char *name, int color, int show);
GeomObject *find_object(ObjectType type, const char *name);

#endif //OBJECT_H