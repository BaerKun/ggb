#include "message.h"
#include "object.h"
#include <stdio.h>

static inline int parse_coord(const char *str, Vec2 *coord) {
  if (sscanf(str, "%f,%f", &coord->x, &coord->y) == 2) return 0;
  throw_error_fmt("'%s' is an invalid coordinate. must be '%%f,%%f'",
                str);
}

int cmd_move_pt(const int argc, const char **argv) {
  if (argc == 1) {
    throw_error("point and destination not provided.");
  }
  if (argc == 2) {
    throw_error("destination not provided.");
  }

  GeomId xy[2];
  propagate_error(object_get_args(POINT, argv[1], xy));

  Vec2 dst;
  propagate_error(parse_coord(argv[2], &dst));

  graph_change_value(2, xy, (const float *)&dst);
  return 0;
}