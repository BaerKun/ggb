#include "object.h"
#include "tool.h"

GeomId create_point(const Vec2 pos, GeomId xy[2]) {
  const Vec2 world = xform_to_world(pos);
  xy[0] = graph_add_value(world.x);
  xy[1] = graph_add_value(world.y);
  const GeomId id = object_create(POINT, xy);
  board_update_buffer();
  return id;
}

GeomId select_point(const Vec2 pos, GeomId xy[2]) {
  const GeomId select = board_select_object(POINT, pos);
  const GeomObject *obj = object_get(select);
  xy[0] = obj->args[0];
  xy[1] = obj->args[1];
  return select;
}