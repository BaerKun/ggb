#include "argparse.h"
#include "message.h"
#include "object.h"
#include <stddef.h>

static Vec2 callback(GeomId argc, const Vec2 *argv) {
  const Vec2 p1 = argv[0];
  const Vec2 p2 = argv[1];
  const Vec2 res = {(p1.x + p2.x) / 2.f, (p1.y + p2.y) / 2.f};
  return res;
}

int midpoint(const int argc, const char **argv) {
  static char *name;
  static int color, hide;
  static struct argparse parse;
  static struct argparse_option opt[] = {
      OPT_STRING('n', "name", &name), OPT_INTEGER('c', "color", &color),
      OPT_BOOLEAN('h', "hide", &hide), OPT_END()};

  name = NULL;
  color = -1;
  hide = 0;
  argparse_init(&parse, opt, NULL, 0);
  const int remaining = argparse_parse(&parse, argc, argv);

  if (remaining < 2) {
    throw_error(MISS_PARAMETER, "2 points needed for midpoint.");
  }

  const char *obj_name = argv[0];
  const GeomObject *obj1 = object_find(POINT, obj_name);
  if (obj1 == NULL) goto point_not_exists;

  obj_name = argv[1];
  const GeomObject *obj2 = object_find(POINT, obj_name);
  if (obj2 == NULL) goto point_not_exists;

  GeomId cons_argv[2] = {obj1->pt1, obj2->pt1};
  GeomId pt = point_create((Vec2){}, (Constraint){2, cons_argv, callback});
  object_create(POINT, pt, -1, name, color, !hide);

  point_not_exists:
  throw_error_fmt(OBJECT_NOT_EXISTS, "point '%s' does not exists.", obj_name);
}
