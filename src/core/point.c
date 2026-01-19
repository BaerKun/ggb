#include "point.h"
#include "cgraph/graph.h"
#include "cgraph/iter.h"

#include <stdlib.h>

typedef struct {
  GeomSize cap;
  GeomSize front, rear;
  GeomId *elems;
} Queue;

static Queue queue;
static CGraph graph;
static PointObject *points;

void point_module_init(const GeomSize init_size) {
  cgraphInit(&graph, true, init_size, init_size * 4);
  points = malloc(init_size * sizeof(PointObject));
  queue.cap = init_size;
  queue.front = queue.rear = 0;
  queue.elems = malloc(sizeof(GeomId) * init_size);
}

void point_module_cleanup() {
  cgraphRelease(&graph);
  free(points);
  free(queue.elems);
}

Vec2 point_get_coord(const GeomId id) { return points[id].coord; }

static void point_coord_calc(PointObject *pt) {
  if (pt->eval == NULL) return;
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

  point_coord_calc(pt);
  return id;
}

void point_delete(const GeomId id) {
  if (id < 0) return;
  const PointObject *pt = points + id;
  for (GeomInt i = 0; i < pt->n_dep; i++) cgraphDeleteEdge(&graph, pt->deps[i]);
  cgraphDeleteVert(&graph, id);
}

void point_move(const GeomId *pts, const Vec2 *dst, const GeomSize count) {
  queue.front = queue.rear = 0;
  if (graph.vertNum > queue.cap) {
    // resize
    queue.cap *= 2;
    void *buff = realloc(queue.elems, sizeof(GeomId) * queue.cap);
    if (buff == NULL) return;
    queue.elems = buff;
  }

  for (GeomSize i = 0; i < count; i++) {
    queue.elems[queue.rear++] = pts[i];
    points[pts[i]].coord = dst[i];
  }

  CGraphIter *iter = cgraphGetIter(&graph);
  while (queue.front != queue.rear) {
    const GeomId pt = queue.elems[queue.front++];
    GeomId eid, to;
    while (cgraphIterNextEdge(iter, pt, &eid, &to)) {
      queue.elems[queue.rear++] = to;
      point_coord_calc(points + to);
    }
  }
  cgraphIterRelease(iter);
}