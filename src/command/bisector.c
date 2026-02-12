#include "command.h"
#include "object.h"
#include <math.h>

static void angle_bisector_ccw(const float inputs[6], float *outputs[3]) {
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

static void create_angle_bisector_ccw(const GeomId inputs[6]) {
  GeomId args[5];
  args[0] = graph_add_value(0);
  args[1] = graph_add_value(0);
  args[2] = graph_add_value(0);
  args[3] = graph_add_value(-HUGE_VALUE);
  args[4] = graph_add_value(HUGE_VALUE);
  graph_add_constraint(6, inputs, 3, args, angle_bisector_ccw);
  object_create(LINE, args);
}

void cmd_bisector(Command * cmd) {
}