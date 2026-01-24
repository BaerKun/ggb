#include "argparse.h"
#include "message.h"
#include "object.h"
#include <string.h>

static Vec2 callback(GeomId argc, const Vec2 *argv) {
  const Vec2 p1 = argv[0];
  const Vec2 p2 = argv[1];
  const Vec2 res = {(p1.x + p2.x) / 2.f, (p1.y + p2.y) / 2.f};
  return res;
}

int cmd_midpoint(const int argc, const char **argv) {
  static char *name;
  static int color;
  static struct argparse parse;
  static struct argparse_option opt[] = {OPT_STRING('n', "name", &name),
                                         OPT_INTEGER('c', "color", &color),
                                         OPT_END()};

  name = NULL, color = -1;
  argparse_init(&parse, opt, NULL, 0);
  const int remaining = argparse_parse(&parse, argc, argv);
  if (remaining < 0) return MSG_ERROR;

  propagate_error(check_name(name));

  if (remaining < 2) throw_error("midpoint need 2 points.");

  GeomId pt1, pt2;
  propagate_error(object_get_points(POINT, argv[0], &pt1, NULL));
  propagate_error(object_get_points(POINT, argv[1], &pt2, NULL));

  GeomId cons_argv[2] = {pt1, pt2};
  const GeomId pt =
      point_create((Vec2){}, (Constraint){2, cons_argv, callback});
  object_create(POINT, pt, -1, name, DEFAULT_COLOR);
  return 0;
}
