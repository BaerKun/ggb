#include "object.h"
#include "tool.h"

static struct {
  int n;
  GeomId first;
  GeomId inputs[4];
} internal = {0, -1};

static int midpoint_eval(const float inputs[4], float *outputs[2]) {
  *outputs[0] = (inputs[0] + inputs[2]) / 2.f;
  *outputs[1] = (inputs[1] + inputs[3]) / 2.f;
  return 1;
}

static void midpoint_reset() {
  if (internal.first != -1) {
    board_deselect_object(internal.first);
    internal.n = 0;
    internal.first = -1;
  }
}

static void midpoint_ctrl(const Vec2 pos, const MouseEvent event) {
  if (event != MOUSE_PRESS) return;

  const GeomId pt = find_or_create_point(pos, internal.inputs + internal.n * 2);
  if (++internal.n == 2) {
    GeomId args[2];
    args[0] = graph_add_value(0);
    args[1] = graph_add_value(0);
    graph_add_constraint(4, internal.inputs, 2, args, midpoint_eval);
    board_add_object(object_create(POINT, args));
    midpoint_reset();
  } else {
    internal.first = pt;
    board_select_object(pt);
  }
}

void tool_midpoint(GeomTool *tool) {
  tool->usage = "midpoint: select two points";
  tool->ctrl = midpoint_ctrl;
  tool->reset = midpoint_reset;
}