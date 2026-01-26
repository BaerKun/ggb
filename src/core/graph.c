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

static CGraph graph;
static GraphNode *nodes;
static GeomId *dep_next;
static Queue queue;

static void graph_vert_resize_callback(const CGraphSize old_cap,
                                       const CGraphSize new_cap) {
  void *mem = realloc(nodes, new_cap * sizeof(GraphNode));
  if (!mem) abort();
  nodes = mem;
  mem = realloc(queue.elems, new_cap * sizeof(GraphNode));
  if (!mem) abort();
  queue.elems = mem;
}

static void graph_edge_resize_callback(const CGraphSize old_cap,
                                       const CGraphSize new_cap) {
  void *mem = realloc(dep_next, new_cap * sizeof(GeomId));
  if (!mem) abort();
  dep_next = mem;
  memset(dep_next + old_cap, -1, (new_cap - old_cap) * sizeof(GeomId));
}

void computation_graph_init(const GeomSize init_size) {
  nodes = malloc(init_size * sizeof(GraphNode));
  dep_next = malloc(init_size * sizeof(GeomId));

  queue.elems = malloc(init_size * sizeof(GeomId));

  cgraphInit(&graph, true, init_size, init_size);
  memset(dep_next, -1, init_size * sizeof(GeomId));
  cgraphSetVertResizeCallback(&graph, graph_vert_resize_callback);
  cgraphSetEdgeResizeCallback(&graph, graph_edge_resize_callback);
}

void computation_graph_cleanup() {
  free(nodes);
  free(dep_next);
  free(queue.elems);
  cgraphRelease(&graph);
}

void computation_graph_clear() {
  memset(dep_next, -1, graph.edgeCap * sizeof(GeomId));
  cgraphClear(&graph);
}

GeomId graph_add_value(const float value) {
  const GeomId id = cgraphAddVert(&graph);
  GraphNode *node = nodes + id;
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
  const CGraphId node_id = cgraphAddVert(&graph);
  GraphNode *node = nodes + node_id;
  node->type = NODE_COMPUTE;
  node->ref_count = (GeomInt)output_size;
  node->scheduled = false;

  float input_values[6], *output_values[6];
  GeomId *pred_ptr = &node->dep_head;
  for (GeomSize i = 0; i < input_size; i++) {
    const GeomId input_id = inputs[i];
    const GeomId eid = cgraphAddEdge(&graph, input_id, node_id, true);
    *pred_ptr = eid;
    pred_ptr = dep_next + eid;
    nodes[input_id].ref_count++;
    input_values[i] = nodes[input_id].value;
  }

  for (GeomSize i = 0; i < output_size; i++) {
    const GeomId output_id = outputs[i];
    const GeomId eid = cgraphPushEdgeBack(&graph, node_id, output_id);
    nodes[output_id].dep_head = eid;
    output_values[i] = &nodes[output_id].value;
  }

  node->eval = eval;
  eval(input_values, output_values);
}

float graph_get_value(const GeomId id) { return nodes[id].value; }

static void enqueue(Queue *q, const GeomId id) { q->elems[q->rear++] = id; }
static GeomId dequeue(Queue *q) { return q->elems[q->front++]; }
static void queue_clear(Queue *q) { q->front = q->rear = 0; }
static bool queue_empty(const Queue *q) { return q->front == q->rear; }

void graph_ref_value(const GeomId id) {
  if (id >= 0) nodes[id].ref_count++;
}

void graph_unref_value(const GeomId id) {
  if (id < 0) return;
  if (--nodes[id].ref_count > 0) return;

  queue_clear(&queue);
  enqueue(&queue, id);
  while (!queue_empty(&queue)) {
    const GeomId node_id = dequeue(&queue);
    for (GeomId dep = nodes[node_id].dep_head; dep != -1; dep = dep_next[dep]) {
      CGraphId from, to;
      cgraphParseEdgeId(&graph, dep, &from, &to);
      cgraphDeleteEdge(&graph, dep);
      if (--nodes[from].ref_count == 0) enqueue(&queue, from);
    }
    cgraphDeleteVert(&graph, node_id);
  }
}

void graph_change_value(const GeomSize count, const GeomId *ids,
                        const float *values) {
  queue_clear(&queue);

  for (GeomSize i = 0; i < count; i++) {
    enqueue(&queue, ids[i]);
    nodes[ids[i]].value = values[i];
  }

  CGraphIter *iter = cgraphGetIter(&graph);
  while (!queue_empty(&queue)) {
    const GeomId id = dequeue(&queue);
    GraphNode *node = nodes + id;
    node->scheduled = false;

    CGraphId eid, to;
    cgraphIterResetEdge(iter, id);
    if (node->type == NODE_VALUE) {
      while (cgraphIterNextEdge(iter, id, &eid, &to)) {
        if (!nodes[to].scheduled) {
          nodes[to].scheduled = true;
          enqueue(&queue, to);
        }
      }
    } else {
      int i = 0;
      float inputs[6], *outputs[6];
      for (CGraphId dep = node->dep_head; dep != -1; dep = dep_next[dep]) {
        CGraphId from;
        cgraphParseEdgeId(&graph, dep, &from, &to);
        inputs[i++] = nodes[from].value;
      }

      i = 0;
      while (cgraphIterNextEdge(iter, id, &eid, &to)) {
        outputs[i++] = &nodes[to].value;
        enqueue(&queue, to);
      }

      node->eval(inputs, outputs);
    }
  }
  cgraphIterRelease(iter);
}