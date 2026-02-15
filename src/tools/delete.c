#include "object.h"
#include "tool.h"

static void delete_ctrl(const Vec2 pos, const MouseEvent event) {
  if (event != MOUSE_PRESS) return;

  const GeomId id = board_find_object(ANY, pos);
  if (id == -1) return;

  object_delete(id);
  board_remove_object(id);
}

void tool_delete(GeomTool *tool) {
  tool->usage = "delete: select object to delete";
  tool->ctrl = delete_ctrl;
  tool->reset = NULL;
}