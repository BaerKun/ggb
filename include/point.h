#ifndef GGB_POINT_H
#define GGB_POINT_H

#include "raymath.h"

typedef struct PointObject_ PointObject;
typedef struct AdjacencyList_ AdjacencyList;

struct AdjacencyList_ {
  PointObject *pt;
  AdjacencyList *next;
};

struct PointObject_ {
  Vector2 coord;
  AdjacencyList *successors;

  int indegree;
  Vector2 (*constraint)(int, const PointObject **);
  const PointObject *predecessors[];
};

typedef struct {
  int argc;
  PointObject **argv;
  Vector2 (*callback)(int, const PointObject **);
} Constraint;

void point_module_init(unsigned init_size);
void point_module_cleanup();
PointObject *point_create(Vector2 coord, Constraint cons);
void point_delete(PointObject *pt);

#endif //GGB_POINT_H