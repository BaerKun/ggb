#include "object.h"
#include "argparse.h"
#include <stdio.h>
#include <string.h>

int create(const int argc, const char **argv) {
  static char *name;
  static int color, hide;
  static struct argparse parse;
  static struct argparse_option opt[] = {
      OPT_STRING('n', "name", &name),
      OPT_INTEGER('c', "color", &color),
      OPT_BOOLEAN('h', "hide", &hide),
      OPT_END()
  };

  name = NULL;
  color = -1;
  hide = 0;
  argparse_init(&parse, opt, NULL, 0);
  const int remaining = argparse_parse(&parse, argc, argv);

  if (remaining == 0) {
    fprintf(stderr, "Error: type not provided.");
    return 1;
  }

  if (name != NULL) {
    if (object_find(ANY, name) != NULL) {
      fprintf(stderr, "Error: name '%s' already exists.", name);
      return 2;
    } else if (strlen(name) > OBJECT_NAME_MAX_LEN) {
      fprintf(stderr, "Error: name '%s' is too long.(<= %d)", name,
              OBJECT_NAME_MAX_LEN);
      return 3;
    }
  }

  const char *type_str = argv[0];
  const ObjectType type = get_type_from_str(type_str);
  if (type == UNKNOWN) {
    fprintf(stderr, "Error: unknown type '%s'.", type_str);
    return 4;
  }

  if ((type == POINT && remaining == 1) || (type != POINT && remaining <= 2)) {
    fprintf(stderr, "Error: miss point arg for '%s'.", type_str);
    return 5;
  }

  Vector2 coord;
  PointObject *pt1, *pt2, *tmp = NULL;

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
    object_create(type, pt1, NULL, name, color, !hide);
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
  fprintf(stderr, "Error: '%s' is an invalid coordinate.", pt_str);
  return 6;

name_not_exist:
  point_delete(tmp);
  fprintf(stderr, "Error: name '%s' does not exist.", pt_str);
  return 7;
}