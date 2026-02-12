#include "command.h"
#include "object.h"

static void midpoint_eval(const float inputs[4], float *outputs[2]) {
  *outputs[0] = (inputs[0] + inputs[2]) / 2.f;
  *outputs[1] = (inputs[1] + inputs[3]) / 2.f;
}

static void select_point(const Vec2 pos, GeomId *args){
  const GeomId select = board_select_object(POINT, pos);
  if (select == -1) {
    const Vec2 world_pos = xform_to_world(pos);
    args[0] = graph_add_value(world_pos.x);
    args[1] = graph_add_value(world_pos.y);
    object_create(POINT, args);
    board_update_buffer();
  } else {
    const GeomObject *obj = object_get(select);
    args[0] = obj->args[0];
    args[1] = obj->args[1];
  }
}

static void midpoint_ctrl(const Vec2 pos, const bool click) {
  if (!click) return;

  static int n = 0;
  static GeomId inputs[4];

  if (n == 0) {
    select_point(pos, inputs);
    n++;
  } else {
    select_point(pos, inputs + 2);
    n--;

    GeomId args[2];
    args[0] = graph_add_value(0);
    args[1] = graph_add_value(0);
    graph_add_constraint(4, inputs, 2, args, midpoint_eval);
    object_create(POINT, args);
  }
}

void cmd_midpoint(Command *cmd){
  cmd->usage = "";
  cmd->ctrl = midpoint_ctrl;
}