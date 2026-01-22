#include "argparse.h"
#include "message.h"
#include "object.h"
#include <string.h>

static int get_point_from_str(const char *str, GeomId *id, Vec2 *coord) {
  if (may_be_coord(str)) {
    if (get_coord_from_str(str, coord)) return 0;
    throw_error_fmt("'%s' isn't an valid coordinate. must be '%%f,%%f'", str);
  }
  const GeomObject *obj = object_find(POINT, str);
  if (!obj) throw_error_fmt("point '%s' doesn't exist.", str);
  *id = obj->pt1;
  return 0;
}

int create(const int argc, const char **argv) {
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

  int code = 0;
  if (check_name(name)) code = MSG_ERROR;

  if (remaining == 0) throw_error("missing type parameter.");

  const char *type_str = argv[0];
  const ObjectType type = get_type_from_str(type_str);
  if (type == UNKNOWN) {
    throw_error_fmt("unknown type '%s'.", type_str);
  }

  if (type == POINT) {
    if (remaining == 1) {
      throw_error("'point' need a coordinate( '%%f,%%f' ).");
    }
  } else if (remaining <= 2) {
    if (type == CIRCLE) {
      throw_error("'circle' need a center and a point on circle.");
    }
    throw_error_fmt("'%s' need 2 points.", type_str);
  }

  Vector2 coord1, coord2;
  GeomId pt1 = -1, pt2 = -1;

  if (get_point_from_str(argv[1], &pt1, &coord1)) code = MSG_ERROR;

  if (type == POINT) {
    if (code) return code;
    if (pt1 == -1) pt1 = point_create(coord1, (Constraint){});
    object_create(type, pt1, pt2, name, color, !hide);
    return 0;
  }

  if (get_point_from_str(argv[2], &pt2, &coord2)) code = MSG_ERROR;

  if (code) return code;
  if (pt1 == -1) pt1 = point_create(coord1, (Constraint){});
  if (pt2 == -1) pt2 = point_create(coord2, (Constraint){});
  object_create(type, pt1, pt2, name, color, !hide);
  return 0;
}