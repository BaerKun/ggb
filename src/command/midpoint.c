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
  static struct argparse_option opt[] = {
      OPT_STRING('n', "name", &name), OPT_INTEGER('c', "color", &color),
       OPT_END()};

  name = NULL;
  color = -1;
  argparse_init(&parse, opt, NULL, 0);
  const int remaining = argparse_parse(&parse, argc, argv);

  int code = 0;
  if (check_name(name)) code = MSG_ERROR;

  if (remaining < 2) throw_error("midpoint need 2 points.");

  const char *obj_name = argv[0];
  const GeomObject *obj1 = object_find(POINT, obj_name);
  if (obj1 == NULL) goto point_not_exists;

  obj_name = argv[1];
  const GeomObject *obj2 = object_find(POINT, obj_name);
  if (obj2 == NULL) goto point_not_exists;

  if (code) return code;

  GeomId cons_argv[2] = {obj1->pt1, obj2->pt1};
  GeomId pt = point_create((Vec2){}, (Constraint){2, cons_argv, callback});
  object_create(POINT, pt, -1, name, color);
  return 0;

  point_not_exists:
  throw_error_fmt("point '%s' doesn't exist.", obj_name);
}
