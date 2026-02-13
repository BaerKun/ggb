#include "object.h"
#include "tool.h"

static struct {
  bool point;
  bool line;
  GeomId inputs[4];
} internal;

static void perp_eval(const float inputs[4], float *output[3]) {
  const float nx = inputs[0];
  const float ny = inputs[1];
  const float px = inputs[2];
  const float py = inputs[3];
  *output[0] = -ny;
  *output[1] = nx;
  *output[2] = -ny * px + nx * py; // np · (px, py)
}

static void perp_foot_eval(const float inputs[4], float *output[2]) {
  const float nx = inputs[0]; // perp line
  const float ny = inputs[1];
  const float d1 = inputs[2]; // perp line
  const float d2 = inputs[3]; // baseline
  *output[0] = nx * d1 + ny * d2;
  *output[1] = ny * d1 - nx * d2;
}

static void clip_end_point(const float inputs[4], float *t[1]) {
  const float nx = inputs[0];
  const float ny = inputs[1];
  const float px = inputs[2];
  const float py = inputs[3];
  *t[0] = ny * px - nx * py;
}


static void perp_init() {
  internal.point = false;
  internal.line = false;
}

static void perp_ctrl(const Vec2 pos, const MouseEvent event) {
  if (event != MOUSE_PRESS) return;

  GeomId id;
  switch (internal.point + internal.line * 2) {
  case 0:
    id = board_select_object(POINT | LINE, pos);
    if (id == -1) {
      create_point(pos, internal.inputs + 2);
      internal.point = true;
    } else {
      const GeomObject *obj = object_get(id);
      if (obj->type == LINE) {
        internal.inputs[0] = obj->args[0];
        internal.inputs[1] = obj->args[1];
        internal.line = true;
      } else {
        internal.inputs[2] = obj->args[0];
        internal.inputs[3] = obj->args[1];
        internal.point = true;
      }
    }
    break;
  case 1:
    id = board_select_object(LINE, pos);
    if (id != -1) {
      const GeomObject *obj = object_get(id);
      internal.inputs[0] = obj->args[0];
      internal.inputs[1] = obj->args[1];
      internal.line = true;
    }
    break;
  case 2:
    id = select_point(pos, internal.inputs + 2);
    if (id == -1) create_point(pos, internal.inputs + 2);
    internal.point = true;
    break;
  default:
    break;
  }

  if (internal.point && internal.line) {
    GeomId args[5];
    args[0] = graph_add_value(0);
    args[1] = graph_add_value(0);
    args[2] = graph_add_value(0);
    args[3] = graph_add_value(-HUGE_VALUE);
    args[4] = graph_add_value(HUGE_VALUE);
    graph_add_constraint(4, internal.inputs, 3, args, perp_eval);
    object_create(LINE, args);
    board_update_buffer();
    perp_init();
  }
}


void tool_perp(GeomTool *tool) {
  tool->usage = "perpendicular line: select line and point";
  tool->init = perp_init;
  tool->ctrl = perp_ctrl;
}