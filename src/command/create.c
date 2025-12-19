#include "object.h"
#include "argparse.h"
#include <stdio.h>
#include <string.h>


int create(const int argc, const char **argv) {
  static char *name;
  static int color, hide, init = 1;
  static struct argparse parse;
  static struct argparse_option opt[] = {
      OPT_STRING('n', "name", &name),
      OPT_INTEGER('c', "color", &color),
      OPT_BOOLEAN('h', "hide", &hide),
      OPT_END()
  };
  if (init) {
    argparse_init(&parse, opt, NULL, 0);
    init = 0;
  }

  name = NULL;
  color = -1;
  hide = 0;
  const int remaining = argparse_parse(&parse, argc, argv);

  if (remaining <= 1) {
    fprintf(stderr, "Error: type not provided.");
    return 1;
  }

  if (name != NULL) {
    if (find_object(ANY, name) != NULL) {
      fprintf(stderr, "Error: name '%s' already exists.", name);
      return 2;
    } else if (strlen(name) > OBJECT_NAME_MAX_LEN) {
      fprintf(stderr, "Error: name '%s' is too long.(<= %d)", name,
              OBJECT_NAME_MAX_LEN);
      return 3;
    }
  }

  const ObjectType type = get_type_from_str(argv[1]);
  if (type == UNKNOWN) {
    fprintf(stderr, "Error: unknown type '%s'.", argv[1]);
    return 4;
  }

  Point2f coord;
  PointObject *pt1, *pt2, *tmp = NULL;

  if (remaining == 2) goto miss_arg;
  const char *str = argv[2];

  if (may_be_coord(str)) {
    if (!get_coord_from_str(str, &coord)) goto coord_err;
    pt1 = tmp = create_point(coord, (Constraint){});
  } else if ((pt1 = find_object(POINT, str)->pt1) == NULL) goto name_not_exist;

  if (type == POINT) {
    create_object(type, pt1, NULL, name, color, !hide);
    return 0;
  }

  if (remaining == 3) goto miss_arg;
  str = argv[3];

  if (may_be_coord(str)) {
    if (!get_coord_from_str(str, &coord)) goto coord_err;
    pt2 = create_point(coord, (Constraint){});
  } else if ((pt2 = find_object(POINT, str)->pt1) == NULL) goto name_not_exist;

  create_object(type, pt1, pt2, name, color, !hide);
  return 0;

miss_arg:
  delete_point(tmp);
  fprintf(stderr, "Error: miss point arg.");
  return 5;

coord_err:
  delete_point(tmp);
  fprintf(stderr, "Error: '%s' is an invalid coordinate.", str);
  return 6;

name_not_exist:
  delete_point(tmp);
  fprintf(stderr, "Error: name '%s' does not exist.", str);
  return 7;
}