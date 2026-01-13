#include "queue.h"

static unsigned points_num = 0;
static Queue queue = {};

void point_module_init(const unsigned init_size) {
  queue_init(&queue, init_size);
}

void point_module_cleanup() {
  queue_free(&queue);
}

PointObject *point_create(const Vec2 coord, const Constraint cons) {
  PointObject *pt = malloc(sizeof(PointObject) + sizeof(PointObject *) * cons.argc);
  pt->coord = coord;
  pt->indegree = cons.argc;
  pt->constraint = cons.callback;
  pt->successors = NULL;
  points_num++;

  for (int i = 0; i < cons.argc; ++i) {
    PointObject *pred = cons.argv[i];
    AdjacencyList *adj = malloc(sizeof(AdjacencyList));

    adj->pt = pt;
    adj->next = pred->successors;
    pred->successors = adj;

    pt->predecessors[i] = pred;
  }
  return pt;
}

void point_delete(PointObject *pt) {
  free(pt);
}

void point_move(PointObject **pts, const Vec2 *dst, const int count) {
  queue_make_empty(&queue);
  queue_resize(&queue, points_num);
}