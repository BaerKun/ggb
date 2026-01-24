#include "message.h"
#include "object.h"
#include <stdio.h>

static inline int get_coord_from_str(const char *str, Vec2 *coord) {
  return sscanf(str, "%f,%f", &coord->x, &coord->y) == 2;
}

int cmd_move_pt(const int argc, const char **argv) {
  if (argc == 1) {
    throw_error("point and destination not provided.");
  }
  if (argc == 2) {
    throw_error("destination not provided.");
  }

  GeomId pt;
  propagate_error(object_get_points(POINT, argv[1], &pt, NULL));

  Vec2 dst;
  if (!get_coord_from_str(argv[2], &dst)) {
    throw_error_fmt("'%s' is an invalid coordinate. must be '%%f,%%f'",
                    argv[2]);
  }

  point_move(&pt, &dst, 1);
  return 0;
}