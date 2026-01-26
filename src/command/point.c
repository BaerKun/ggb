#include "object.h"
#include "message.h"
#include "argparse.h"
#include <stdlib.h>

int cmd_point(const int argc, const char **argv) {
  static char *name, *color_str;
  static struct argparse parse;
  static struct argparse_option opt[] = {OPT_STRING('n', "name", &name),
                                         OPT_STRING('c', "color", &color_str),
                                         OPT_END()};

  name = color_str = NULL;
  argparse_init(&parse, opt, NULL, 0);
  const int remaining = argparse_parse(&parse, argc, argv);
  if (remaining < 0) return MSG_ERROR;

  int32_t color;
  propagate_error(parse_color(color_str, &color));
  propagate_error(check_name(name));

  if (remaining < 2) {
    throw_error("point <x> <y>");
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

  const GeomId arg_x = graph_add_value(x);
  const GeomId arg_y = graph_add_value(y);
  const GeomId args[] = {arg_x, arg_y};
  object_create(POINT, args, name, color);
  return 0;
}