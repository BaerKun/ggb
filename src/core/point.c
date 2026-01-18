#include "point.h"
#include "cgraph/graph.h"
#include <stdlib.h>

typedef struct {
  GeomSize capacity, size;
  GeomSize front, rear;
  PointObject **elements;
} Queue;

static Queue queue;
static CGraph graph;
static PointObject *points;

void point_module_init(const GeomSize init_size) {
  cgraphInit(&graph, true, init_size, init_size * 4);
  points = malloc(init_size * sizeof(PointObject));
  queue.capacity = init_size;
  queue.size = queue.front = queue.rear = 0;
  queue.elements = malloc(sizeof(PointObject *) * init_size);
}

void point_module_cleanup() {
  cgraphRelease(&graph);
  free(points);
  free(queue.elements);
}

Vec2 point_get_coord(const GeomId id) { return points[id].coord; }

static void point_coord_calc(PointObject *pt) {
  Vec2 pred[6];
  for (GeomInt i = 0; i < pt->n_dep; i++) pred[i] = points[pt->deps[i]].coord;
  pt->coord = pt->eval(pt->n_dep, pred);
}

GeomId point_create(const Vec2 coord, const Constraint cons) {
  const GeomId id = cgraphAddVert(&graph);
  PointObject *pt = points + id;
  pt->coord = coord;
  pt->n_dep = cons.n_pts;
  pt->eval = cons.eval;

  for (GeomInt i = 0; i < cons.n_pts; i++) {
    cgraphAddEdge(&graph, cons.pts[i], id, true);
    pt->deps[i] = cons.pts[i];
  }

  if (pt->eval) point_coord_calc(pt);
  return id;
}

void point_delete(const GeomId id) {
  const PointObject *pt = points + id;
  for (GeomInt i = 0; i < pt->n_dep; i++) cgraphDeleteEdge(&graph, pt->deps[i]);
  cgraphDeleteVert(&graph, id);
}

static inline void enqueue(PointObject *const element) {
  queue.elements[queue.rear] = element;
  if (++queue.rear == queue.capacity) queue.rear = 0;
  queue.size++;
}

static inline PointObject *dequeue() {
  PointObject *const front = queue.elements[queue.front];
  if (++queue.front == queue.capacity) queue.front = 0;
  queue.size--;
  return front;
}

static inline void queue_resize(const unsigned new_cap) {
  if (new_cap <= queue.capacity) return;
  queue.capacity = new_cap + queue.capacity; // * 2
  void *buff = realloc(queue.elements, sizeof(PointObject *) * new_cap);
  if (buff == NULL) return;
  queue.elements = buff;
}

void point_move(PointObject **pts, const Vec2 *dst, const int count) {}