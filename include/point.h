#ifndef GGB_POINT_H
#define GGB_POINT_H

#include "math_.h"

typedef struct PointObject_ PointObject;
typedef struct AdjacencyList_ AdjacencyList;

struct AdjacencyList_ {
  PointObject *pt;
  AdjacencyList *next;
};

struct PointObject_ {
  Vec2 coord;
  AdjacencyList *successors;

  int indegree;
  Vec2 (*constraint)(int, PointObject **);
  const PointObject *predecessors[];
};

typedef struct {
  int argc;
  PointObject **argv;
  Vec2 (*callback)(int, PointObject **);
} Constraint;

void point_module_init(unsigned init_size);
void point_module_cleanup();
PointObject *point_create(Vec2 coord, Constraint cons);
void point_delete(PointObject *pt);

#endif //GGB_POINT_H