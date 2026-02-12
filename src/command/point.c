#include "command.h"
#include "object.h"

static void point_ctrl(const Vec2 pos, const bool click) {
  if (!click) return;

  const GeomId select = board_select_object(POINT, pos);
  if (select != -1) return;

  const Vec2 world_pos = xform_to_world(pos);

  GeomId args[2];
  args[0] = graph_add_value(world_pos.x);
  args[1] = graph_add_value(world_pos.y);
  object_create(POINT, args);
  board_update_buffer();
}

void cmd_point(Command *cmd) {
  cmd->usage = "";
  cmd->ctrl = point_ctrl;
}