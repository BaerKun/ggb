#include "argparse.h"
#include "message.h"
#include "object.h"

int cmd_line(const int argc, const char **argv) {
  static char *name;
  static int color, seg, ray;
  static struct argparse parse;
  static struct argparse_option opt[] = {
      OPT_STRING('n', "name", &name), OPT_INTEGER('c', "color", &color),
      OPT_BOOLEAN(0, "seg", &seg), OPT_BOOLEAN(0, "ray", &ray), OPT_END()};

  name = NULL, color = -1, ray = seg = 0;
  argparse_init(&parse, opt, NULL, 0);
  const int remaining = argparse_parse(&parse, argc, argv);
  if (remaining < 0) return MSG_ERROR;

  propagate_error(check_name(name));

  if (remaining < 2) {
    throw_error("'line' need 2 points. [--ray] [--seg]");
  }

  GeomId pt1, pt2;
  propagate_error(object_get_points(POINT, argv[0], &pt1, NULL));
  propagate_error(object_get_points(POINT, argv[1], &pt2, NULL));

  ObjectType type = LINE;
  if (ray) type = RAY;
  if (seg) type = SEG;
  object_create(type, pt1, pt2, name, DEFAULT_COLOR);
  return 0;
}