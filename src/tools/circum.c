#include "object.h"
#include "tool.h"
#include <math.h>

static struct {
  int n;
  GeomId points[2];
  GeomId inputs[6];
} internal = {0};

static int circum_eval(const float inputs[6], float *outputs[3]) {
  const float x1 = inputs[0], y1 = inputs[1];
  const float x2 = inputs[2], y2 = inputs[3];
  const float x3 = inputs[4], y3 = inputs[5];
  const float D = 2 * (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2));
  if (fabsf(D) < EPS) return 0;

  const float sq_xy1 = x1 * x1 + y1 * y1;
  const float sq_xy2 = x2 * x2 + y2 * y2;
  const float sq_xy3 = x3 * x3 + y3 * y3;
  const float cx =
      (sq_xy1 * (y2 - y3) + sq_xy2 * (y3 - y1) + sq_xy3 * (y1 - y2)) / D;
  const float cy =
      (sq_xy1 * (x3 - x2) + sq_xy2 * (x1 - x3) + sq_xy3 * (x2 - x1)) / D;
  const float rx = x1 - cx;
  const float ry = y1 - cy;
  *outputs[0] = cx;
  *outputs[1] = cy;
  *outputs[2] = sqrtf(rx * rx + ry * ry);
  return 1;
}

static void circum_reset() {
  switch (internal.n) {
  case 2:
    board_deselect_object(internal.points[1]);
    internal.points[1] = -1;
  case 1:
    board_deselect_object(internal.points[0]);
    internal.points[0] = -1;
    internal.n = 0;
  default:
    break;
  }
}

static void circum_ctrl(const Vec2 pos, const MouseEvent event) {
  if (event != MOUSE_PRESS) return;

  const GeomId pt = find_or_create_point(pos, internal.inputs + internal.n * 2);
  if (internal.n + 1 == 3) {
    GeomId args[3];
    args[0] = graph_add_value(0);
    args[1] = graph_add_value(0);
    args[2] = graph_add_value(0);
    const GeomId define =
        graph_add_constraint(6, internal.inputs, 3, args, circum_eval);
    board_add_object(object_create(CIRCLE, args, define, 0));
    circum_reset();
  } else {
    internal.points[internal.n++] = pt;
    board_select_object(pt);
  }
}

void tool_circum(GeomTool *tool) {
  tool->usage = "circumcircle: select three points";
  tool->ctrl = circum_ctrl;
  tool->reset = circum_reset;
}