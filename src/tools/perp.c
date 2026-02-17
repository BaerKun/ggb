#include "object.h"
#include "tool.h"

static struct {
  ObjectType first_t;
  GeomId first_id;
  GeomId inputs[4];
} internal = {UNKNOWN, -1};

static bool perp_eval(const float inputs[4], float *output[3]) {
  const float nx = inputs[0];
  const float ny = inputs[1];
  const float px = inputs[2];
  const float py = inputs[3];
  *output[0] = -ny;
  *output[1] = nx;
  *output[2] = -ny * px + nx * py; // np · (px, py)
  return true;
}

static void perp_reset() {
  if (internal.first_id != -1){
    board_deselect_object(internal.first_id);
    internal.first_t = UNKNOWN;
    internal.first_id = -1;
  }
}

static void perp_ctrl(const Vec2 pos, const MouseEvent event) {
  if (event != MOUSE_PRESS) return;

  GeomId id;
  if (internal.first_id == -1) {
    id = board_find_object(POINT | LINE, pos);
    if (id == -1) {
      id = create_point(pos, internal.inputs + 2);
      internal.first_t = POINT;
    } else {
      const GeomObject *obj = object_get(id);
      if (obj->type == LINE) {
        internal.first_t = LINE;
        copy_args(internal.inputs, obj->args, 2);
      } else {
        internal.first_t = POINT;
        copy_args(internal.inputs + 2, obj->args, 2);
      }
    }
    internal.first_id = id;
    board_select_object(id);
    return;
  }

  if (internal.first_t == POINT) {
    id = board_find_object(LINE, pos);
    if (id != -1) {
      const GeomObject *obj = object_get(id);
      copy_args(internal.inputs, obj->args, 2);
    }
  } else {
    find_or_create_point(pos, internal.inputs + 2);
  }

  GeomId args[5];
  init_line(args);
  graph_add_constraint(4, internal.inputs, 3, args, perp_eval);
  board_add_object(object_create(LINE, args));
  perp_reset();
}

void tool_perp(GeomTool *tool) {
  tool->usage = "perpendicular line: select line and point";
  tool->ctrl = perp_ctrl;
  tool->reset = perp_reset;
}