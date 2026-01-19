#include "argparse.h"
#include "message.h"
#include "object.h"
#include <string.h>

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

  if (remaining == 0) {
    throw_error(MISS_PARAMETER, "missing type parameter.");
  }

  if (name != NULL) {
    if (strlen(name) > OBJECT_NAME_MAX_LEN) {
      throw_error_fmt(NAME_TOO_LONG, "name '%s' is too long. ( <= %d )", name,
                      OBJECT_NAME_MAX_LEN);
    } else if (object_find(ANY, name) != NULL) {
      throw_error_fmt(NAME_EXISTS, "name '%s' already exists.", name);
    }
  }

  const char *type_str = argv[0];
  const ObjectType type = get_type_from_str(type_str);
  if (type == UNKNOWN) {
    throw_error_fmt(UNKNOWN_PARAMETER, "unknown type '%s'.", type_str);
  }

  if (type == POINT && remaining == 1) {
    throw_error(MISS_PARAMETER, "miss coordinate for '%s'.");
  } else if (type != POINT && remaining <= 2) {
    throw_error_fmt(MISS_PARAMETER, "2 points needed for '%s'.", type_str);
  }

  Vector2 coord;
  GeomId pt1, pt2, tmp = -1;

  const char *pt_str = argv[1];
  if (may_be_coord(pt_str)) {
    if (!get_coord_from_str(pt_str, &coord)) goto coord_err;
    pt1 = tmp = point_create(coord, (Constraint){});
  } else {
    const GeomObject *obj = object_find(POINT, pt_str);
    if (obj == NULL) goto name_not_exist;
    pt1 = obj->pt1;
  }

  if (type == POINT) {
    object_create(type, pt1, -1, name, color, !hide);
    return 0;
  }

  pt_str = argv[2];
  if (may_be_coord(pt_str)) {
    if (!get_coord_from_str(pt_str, &coord)) goto coord_err;
    pt2 = point_create(coord, (Constraint){});
  } else {
    const GeomObject *obj = object_find(POINT, pt_str);
    if (obj == NULL) goto name_not_exist;
    pt2 = obj->pt1;
  }

  object_create(type, pt1, pt2, name, color, !hide);
  return 0;

coord_err:
  point_delete(tmp);
  throw_error_fmt(INVALID_PARAMETER, "'%s' is an invalid coordinate.", pt_str);

name_not_exist:
  point_delete(tmp);
  throw_error_fmt(OBJECT_NOT_EXISTS, "point '%s' not exists.", pt_str);
}