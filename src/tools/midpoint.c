#include "object.h"
#include "tool.h"

static struct {
  int n;
  GeomId inputs[4];
} internal;

static void midpoint_eval(const float inputs[4], float *outputs[2]) {
  *outputs[0] = (inputs[0] + inputs[2]) / 2.f;
  *outputs[1] = (inputs[1] + inputs[3]) / 2.f;
}

static void midpoint_init() {
  internal.n = 0;
}

static void midpoint_ctrl(const Vec2 pos, const MouseEvent event) {
  if (event != MOUSE_PRESS) return;

  const GeomId id = select_point(pos, internal.inputs + internal.n * 2);
  if (id == -1) create_point(pos, internal.inputs + internal.n * 2);
  if (++internal.n == 2) {
    GeomId args[2];
    args[0] = graph_add_value(0);
    args[1] = graph_add_value(0);
    graph_add_constraint(4, internal.inputs, 2, args, midpoint_eval);
    object_create(POINT, args);
    board_update_buffer();
    internal.n = 0;
  }
}

void tool_midpoint(GeomTool *tool) {
  tool->usage = "midpoint: select two points";
  tool->init = midpoint_init;
  tool->ctrl = midpoint_ctrl;
}