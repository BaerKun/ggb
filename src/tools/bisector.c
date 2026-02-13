#include "object.h"
#include "tool.h"
#include <math.h>

static struct {
  int n;
  GeomId inputs[6];
} internal;

static void bisector_ccw_eval(const float inputs[6], float *outputs[3]) {
  const float nx1 = inputs[0];
  const float ny1 = inputs[1];
  const float dd1 = inputs[2];
  const float nx2 = inputs[3];
  const float ny2 = inputs[4];
  const float dd2 = inputs[5];
  const float cross = nx1 * ny2 - ny1 * nx2;
  const float nx = cross > 0 ? nx1 + nx2 : nx1 - nx2;
  const float ny = cross > 0 ? ny1 + ny2 : ny1 - ny2;
  const float dd = cross > 0 ? dd1 + dd2 : dd1 - dd2;
  const float norm = sqrtf(nx * nx + ny * ny);
  *outputs[0] = nx / norm;
  *outputs[1] = ny / norm;
  *outputs[2] = dd / norm;
}

static void create_bisector_ccw(const GeomId inputs[6]) {
  GeomId args[5];
  args[0] = graph_add_value(0);
  args[1] = graph_add_value(0);
  args[2] = graph_add_value(0);
  args[3] = graph_add_value(-HUGE_VALUE);
  args[4] = graph_add_value(HUGE_VALUE);
  graph_add_constraint(6, inputs, 3, args, bisector_ccw_eval);
  object_create(LINE, args);
}

static void create_bisector_cw(const GeomId inputs[6]) {
  const GeomId inputs2[6] = {inputs[3], inputs[4], inputs[5],
                             inputs[0], inputs[1], inputs[2]};
  create_bisector_ccw(inputs2);
}

static GeomId select_line(const Vec2 pos, GeomId *args) {
  const GeomId id = board_select_object(LINE, pos);
  if (id != -1) {
    const GeomObject *obj = object_get(id);
    args[0] = obj->args[0];
    args[1] = obj->args[1];
    args[2] = obj->args[2];
  }
  return id;
}

static void bisector_init() { internal.n = 0; }

static void bisector_ctrl(const Vec2 pos, const MouseEvent event) {
  if (event != MOUSE_PRESS) return;

  const GeomId line = select_line(pos, internal.inputs + internal.n * 3);
  if (line == -1) return;

  if (++internal.n == 2) {
    create_bisector_ccw(internal.inputs);
    create_bisector_cw(internal.inputs);
    board_update_buffer();
    internal.n = 0;
  }
}
void tool_bisector(GeomTool *tool) {
  tool->usage = "angle bisector: select two lines";
  tool->init = bisector_init;
  tool->ctrl = bisector_ctrl;
}