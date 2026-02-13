#include "object.h"
#include "tool.h"
#include <math.h>

static struct {
  int n;
  GeomId inputs[4];
} internal;

static void line_eval(const float xyxy[4], float *line[3]) {
  const float x1 = xyxy[0];
  const float y1 = xyxy[1];
  const float x2 = xyxy[2];
  const float y2 = xyxy[3];
  const float dx = x2 - x1;
  const float dy = y2 - y1;
  const float dist = sqrtf(dx * dx + dy * dy);
  if (dist < EPS) return;

  const float nx = -dy / dist;
  const float ny = dx / dist;
  *line[0] = nx;
  *line[1] = ny;
  *line[2] = nx * x1 + ny * y1; // dd = n · (x, y)
}

static void clip_end_point(const float inputs[4], float *t[1]) {
  const float nx = inputs[0];
  const float ny = inputs[1];
  const float px = inputs[2];
  const float py = inputs[3];
  *t[0] = ny * px - nx * py; // (ny, -nx) · (px, py)
}

static void line_init() {
  internal.n = 0;
}

static void line_ctrl(const Vec2 pos, const MouseEvent event) {
  if (event != MOUSE_PRESS) return;

  const GeomId id = select_point(pos, internal.inputs + internal.n * 2);
  if (id == -1) create_point(pos, internal.inputs + internal.n * 2);
  if (++internal.n == 2) {
    GeomId args[5];
    args[0] = graph_add_value(0);
    args[1] = graph_add_value(0);
    args[2] = graph_add_value(0);
    args[3] = graph_add_value(-HUGE_VALUE);
    args[4] = graph_add_value(HUGE_VALUE);
    graph_add_constraint(4, internal.inputs, 3, args, line_eval);
    object_create(LINE, args);
    board_update_buffer();
    internal.n = 0;
  }
}

void tool_line(GeomTool *tool) {
  tool->usage = "line: select two points or positions";
  tool->init = line_init;
  tool->ctrl = line_ctrl;
}