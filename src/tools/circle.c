#include "object.h"
#include "tool.h"
#include <math.h>

static struct {
  int n;
  GeomId inputs[4];
} internal;

static void circle_point_eval(const float xyxy[4], float *radius[1]) {
  const float dx = xyxy[2] - xyxy[0];
  const float dy = xyxy[3] - xyxy[1];
  *radius[0] = sqrtf(dx * dx + dy * dy);
}

static void circle_init() {
  internal.n = 0;
}

static void circle_ctrl(Vec2 pos, const MouseEvent event) {
  if (event != MOUSE_PRESS) return;

  const GeomId id = select_point(pos, internal.inputs + internal.n * 2);
  if (id == -1) create_point(pos, internal.inputs + internal.n * 2);
  if (++internal.n == 2) {
    GeomId args[3];
    args[0] = internal.inputs[0];
    args[1] = internal.inputs[1];
    args[2] = graph_add_value(0);
    graph_add_constraint(4, internal.inputs, 1, args + 2, circle_point_eval);
    object_create(CIRCLE, args);
    board_update_buffer();
    internal.n = 0;
  }
}

void tool_circle(GeomTool *tool) {
  tool->usage = "circle: select center point, then point on circle";
  tool->init = circle_init;
  tool->ctrl = circle_ctrl;
}