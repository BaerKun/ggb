#include "object.h"
#include "tool.h"

static void point_ctrl(const Vec2 pos, const MouseEvent event) {
  if (event != MOUSE_PRESS) return;

  const GeomId id = board_find_object(POINT, pos);
  if (id != -1) return;

  const Vec2 world_pos = xform_to_world(pos);
  GeomId args[2];
  args[0] = graph_add_value(world_pos.x);
  args[1] = graph_add_value(world_pos.y);
  board_add_object(object_create(POINT, args));
}

void tool_point(GeomTool *tool) {
  tool->usage = "point: select position";
  tool->ctrl = point_ctrl;
  tool->reset = NULL;
}