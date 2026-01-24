#include "point.h"
#include "cgraph/graph.h"
#include "cgraph/iter.h"
#include <stdlib.h>

typedef struct {
  GeomSize front, rear;
  GeomId *elems;
} Queue;

static struct {
  PointObject *points;
  Queue queue;
  CGraph graph;
} global;

static void vertex_resize_callback(const CGraphSize new_cap) {
  void *mem = realloc(global.points, new_cap * sizeof(PointObject));
  if (!mem) abort();
  global.points = mem;
  mem = realloc(global.queue.elems, new_cap * sizeof(GeomId));
  if (!mem) abort();
  global.queue.elems = mem;
}

void point_module_init(const GeomSize init_size) {
  global.points = malloc(init_size * sizeof(PointObject));
  global.queue.elems = malloc(init_size * sizeof(GeomId));
  cgraphInit(&global.graph, true, init_size, init_size * 2);
  cgraphSetVertResizeCallback(&global.graph, vertex_resize_callback);
}

void point_module_cleanup() {
  free(global.points);
  free(global.queue.elems);
  cgraphRelease(&global.graph);
}

Vec2 point_get_coord(const GeomId id) { return global.points[id].coord; }

static void point_coord_calc(PointObject *pt) {
  if (pt->eval == NULL) return;
  Vec2 pred[6];
  for (GeomInt i = 0; i < pt->n_dep; i++) {
    pred[i] = global.points[pt->deps[i]].coord;
  }
  pt->coord = pt->eval(pt->n_dep, pred);
}

GeomId point_create(const Vec2 coord, const Constraint cons) {
  const GeomId id = cgraphAddVert(&global.graph);
  PointObject *pt = global.points + id;
  pt->coord = coord;
  pt->shared = 0;
  pt->n_dep = cons.n_pts;
  pt->eval = cons.eval;

  for (GeomInt i = 0; i < cons.n_pts; i++) {
    const GeomId dep_pt = cons.pts[i];
    cgraphAddEdge(&global.graph, dep_pt, id, true);
    pt->deps[i] = dep_pt;
    global.points[dep_pt].shared++;
  }

  point_coord_calc(pt);
  return id;
}

void point_clear() {
  cgraphClear(&global.graph);
}

static void queue_clean(Queue *q) { q->front = q->rear = 0; }
static bool queue_empty(const Queue *q) { return q->front == q->rear; }
static void enqueue(Queue *q, const GeomId id) { q->elems[q->rear++] = id; }
static GeomId dequeue(Queue *q) { return q->elems[q->front++]; }

void point_ref(const GeomId id) {
  if (id >= 0) global.points[id].shared++;
}

void point_unref(const GeomId id) {
  if (id < 0) return;
  if (--global.points[id].shared > 0) return;

  CGraph *graph = &global.graph;
  Queue *queue = &global.queue;
  queue_clean(queue);
  enqueue(queue, id);
  while (!queue_empty(queue)) {
    const GeomId pt_id = dequeue(queue);
    cgraphDeleteVert(graph, pt_id);

    const PointObject *pt_obj = global.points + pt_id;
    for (GeomInt i = 0; i < pt_obj->n_dep; i++) {
      const GeomId dep_id = pt_obj->deps[i];
      PointObject *dep_obj = global.points + dep_id;
      cgraphDeleteEdge(graph, cgraphFindEdgeId(graph, dep_id, pt_id));
      if (--dep_obj->shared == 0) enqueue(queue, dep_id);
    }
  }
}

void point_move(const GeomId *pts, const Vec2 *dst, const GeomSize count) {
  Queue *queue = &global.queue;
  queue_clean(queue);

  for (GeomSize i = 0; i < count; i++) {
    enqueue(queue, pts[i]);
    global.points[pts[i]].coord = dst[i];
  }

  CGraphIter *iter = cgraphGetIter(&global.graph);
  while (!queue_empty(queue)) {
    const GeomId pt = dequeue(queue);
    point_coord_calc(global.points + pt);
    GeomId eid, to;
    while (cgraphIterNextEdge(iter, pt, &eid, &to)) {
      enqueue(queue, to);
    }
  }
  cgraphIterRelease(iter);
}