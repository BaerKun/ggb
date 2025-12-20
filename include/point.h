#ifndef GGB_POINT_H
#define GGB_POINT_H

#include "geometry.h"

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

void point_module_init(unsigned init_size);
void point_module_cleanup();
PointObject *point_create(Point2f coord, Constraint cons);
void point_delete(PointObject *pt);

#endif //GGB_POINT_H