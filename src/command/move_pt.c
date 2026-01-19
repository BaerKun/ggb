#include "object.h"
#include "message.h"

int move_pt(const int argc, const char **argv) {
  if (argc == 1) {
    throw_error(MISS_PARAMETER, "point and destination not provided.");
  }
  if (argc == 2) {
    throw_error(MISS_PARAMETER, "destination not provided.");
  }
  const GeomObject *obj = object_find(POINT, argv[1]);
  if (obj == NULL) {
    throw_error_fmt(OBJECT_NOT_EXISTS, "point '%s' not exists.", argv[1]);
  }

  Vec2 dst;
  if (!get_coord_from_str(argv[2], &dst)) {
    throw_error_fmt(INVALID_PARAMETER, "'%s' is an invalid coordinate.",
                    argv[2]);
  }

  point_move(&obj->pt1, &dst, 1);
  return 0;
}