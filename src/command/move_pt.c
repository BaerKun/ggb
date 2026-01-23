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
  const GeomObject *obj = object_find(POINT, argv[1]);
  if (obj == NULL) {
    throw_error_fmt("point '%s' not exists.", argv[1]);
  }

  Vec2 dst;
  if (!get_coord_from_str(argv[2], &dst)) {
    throw_error_fmt("'%s' is an invalid coordinate. must be '%%f,%%f'",
                    argv[2]);
  }

  point_move(&obj->pt1, &dst, 1);
  return 0;
}