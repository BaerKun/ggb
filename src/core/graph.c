#include "cgraph/graph.h"
#include "cgraph/iter.h"
#include "graph.h"
#include <stdlib.h>
#include <string.h>

typedef enum { NODE_VALUE = 1, NODE_COMPUTE = 2, NODE_BOTH = 3 } NodeType;

typedef struct {
  NodeType type;
  float value;
  GeomInt ref_count;

  bool scheduled;
  GeomId dep_head; // eid
  ValueEval eval;
} GraphNode;

typedef struct {
  GeomSize front, rear;
  GeomId *elems;
} Queue;

static struct {
  CGraph graph;
  GraphNode *nodes;
  GeomId *dep_next;
  Queue queue;
} internal;

static void graph_vert_resize_callback(CGraphSize, CGraphSize);
static void graph_edge_resize_callback(CGraphSize, CGraphSize);
static void enqueue(Queue *q, const GeomId id) { q->elems[q->rear++] = id; }
static GeomId dequeue(Queue *q) { return q->elems[q->front++]; }
static void queue_clear(Queue *q) { q->front = q->rear = 0; }
static bool queue_empty(const Queue *q) { return q->front == q->rear; }

void computation_graph_init(const GeomSize init_size) {
  internal.nodes = malloc(init_size * sizeof(GraphNode));
  internal.dep_next = malloc(init_size * sizeof(GeomId));
  internal.queue.elems = malloc(init_size * sizeof(GeomId));

  cgraphInit(&internal.graph, true, init_size, init_size);
  memset(internal.dep_next, -1, init_size * sizeof(GeomId));
  cgraphSetVertResizeCallback(&internal.graph, graph_vert_resize_callback);
  cgraphSetEdgeResizeCallback(&internal.graph, graph_edge_resize_callback);
}

void computation_graph_cleanup() {
  free(internal.nodes);
  free(internal.dep_next);
  free(internal.queue.elems);
  cgraphRelease(&internal.graph);
}

void computation_graph_clear() {
  memset(internal.dep_next, -1, internal.graph.edgeCap * sizeof(GeomId));
  cgraphClear(&internal.graph);
}

GeomId graph_add_value(const float value) {
  const GeomId id = cgraphAddVert(&internal.graph);
  GraphNode *node = internal.nodes + id;
  node->type = NODE_VALUE;
  node->value = value;
  node->ref_count = 0;
  node->scheduled = false;
  node->dep_head = -1;
  node->eval = NULL;
  return id;
}

void graph_add_constraint(const GeomSize input_size, const GeomId *inputs,
                          const GeomSize output_size, const GeomId *outputs,
                          const ValueEval eval) {
  const CGraphId node_id = cgraphAddVert(&internal.graph);
  GraphNode *node = internal.nodes + node_id;
  node->type = NODE_COMPUTE;
  node->ref_count = (GeomInt)output_size;
  node->scheduled = false;

  float input_values[6], *output_values[6];
  GeomId *pred_ptr = &node->dep_head;
  for (GeomSize i = 0; i < input_size; i++) {
    const GeomId input_id = inputs[i];
    const GeomId eid = cgraphAddEdge(&internal.graph, input_id, node_id, true);
    *pred_ptr = eid;
    pred_ptr = internal.dep_next + eid;
    internal.nodes[input_id].ref_count++;
    input_values[i] = internal.nodes[input_id].value;
  }

  for (GeomSize i = 0; i < output_size; i++) {
    const GeomId output_id = outputs[i];
    const GeomId eid = cgraphPushEdgeBack(&internal.graph, node_id, output_id);
    internal.nodes[output_id].dep_head = eid;
    output_values[i] = &internal.nodes[output_id].value;
  }

  node->eval = eval;
  eval(input_values, output_values);
}

float graph_get_value(const GeomId id) { return internal.nodes[id].value; }

void graph_ref_value(const GeomId id) {
  if (id >= 0) internal.nodes[id].ref_count++;
}

void graph_unref_value(const GeomSize count, const GeomId *ids) {
  queue_clear(&internal.queue);
  for (GeomSize i = 0; i < count; i++) {
    const GeomId id = ids[i];
    if (id < 0) continue;
    if (--internal.nodes[id].ref_count <= 0) enqueue(&internal.queue, id);
  }

  while (!queue_empty(&internal.queue)) {
    const GeomId node_id = dequeue(&internal.queue);
    for (GeomId dep = internal.nodes[node_id].dep_head; dep != -1;
         dep = internal.dep_next[dep]) {
      CGraphId from, to;
      cgraphParseEdgeId(&internal.graph, dep, &from, &to);
      cgraphDeleteEdge(&internal.graph, dep);
      if (--internal.nodes[from].ref_count == 0) enqueue(&internal.queue, from);
    }
    cgraphDeleteVert(&internal.graph, node_id);
  }
}

void graph_change_value(const GeomSize count, const GeomId *ids,
                        const float *values) {
  queue_clear(&internal.queue);

  for (GeomSize i = 0; i < count; i++) {
    enqueue(&internal.queue, ids[i]);
    internal.nodes[ids[i]].value = values[i];
  }

  CGraphIter *iter = cgraphGetIter(&internal.graph);
  while (!queue_empty(&internal.queue)) {
    const GeomId id = dequeue(&internal.queue);
    GraphNode *node = internal.nodes + id;
    node->scheduled = false;

    CGraphId eid, to;
    cgraphIterResetEdge(iter, id);
    if (node->type == NODE_VALUE) {
      while (cgraphIterNextEdge(iter, id, &eid, &to)) {
        if (!internal.nodes[to].scheduled) {
          internal.nodes[to].scheduled = true;
          enqueue(&internal.queue, to);
        }
      }
    } else {
      int i = 0;
      float inputs[6], *outputs[6];
      for (CGraphId dep = node->dep_head; dep != -1;
           dep = internal.dep_next[dep]) {
        CGraphId from;
        cgraphParseEdgeId(&internal.graph, dep, &from, &to);
        inputs[i++] = internal.nodes[from].value;
      }

      i = 0;
      while (cgraphIterNextEdge(iter, id, &eid, &to)) {
        outputs[i++] = &internal.nodes[to].value;
        enqueue(&internal.queue, to);
      }

      node->eval(inputs, outputs);
    }
  }
  cgraphIterRelease(iter);
}

static void graph_vert_resize_callback(const CGraphSize old_cap,
                                       const CGraphSize new_cap) {
  void *mem = realloc(internal.nodes, new_cap * sizeof(GraphNode));
  if (!mem) abort();
  internal.nodes = mem;
  mem = realloc(internal.queue.elems, new_cap * sizeof(GraphNode));
  if (!mem) abort();
  internal.queue.elems = mem;
}

static void graph_edge_resize_callback(const CGraphSize old_cap,
                                       const CGraphSize new_cap) {
  void *mem = realloc(internal.dep_next, new_cap * sizeof(GeomId));
  if (!mem) abort();
  internal.dep_next = mem;
  memset(internal.dep_next + old_cap, -1, (new_cap - old_cap) * sizeof(GeomId));
}
