#include "object.h"
#include "message.h"
#include "argparse.h"
#include <stdlib.h>

int cmd_point(const int argc, const char **argv) {
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

  if (remaining < 2) {
    throw_error("point need x and y value.");
  }

  char *end;
  const float x = strtof(argv[0], &end);
  if (*end) {
    throw_error_fmt("x value should be a number. got '%s'.", argv[0]);
  }

  const float y = strtof(argv[1], &end);
  if (*end) {
    throw_error_fmt("y value should be a number. got '%s'.", argv[1]);
  }

  const GeomId pt = point_create((Vec2){x, y}, (Constraint){});
  object_create(POINT, pt, -1, name, color);
  return 0;
}