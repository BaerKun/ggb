#include "argparse.h"
#include "message.h"
#include "object.h"
#include <string.h>

static void midpoint(const float inputs[4], float *outputs[2]) {
  *outputs[0] = (inputs[0] + inputs[2]) / 2.f;
  *outputs[1] = (inputs[1] + inputs[3]) / 2.f;
}

int cmd_midpoint(const int argc, const char **argv) {
  static char *name, *color_str;
  static struct argparse parse;
  static struct argparse_option opt[] = {OPT_STRING('n', "name", &name),
                                         OPT_STRING('c', "color", &color_str),
                                         OPT_END()};

  name = color_str = NULL;
  argparse_init(&parse, opt, NULL, 0);
  const int remaining = argparse_parse(&parse, argc, argv);
  if (remaining < 0) return MSG_ERROR;

  Color color;
  propagate_error(parse_color(color_str, &color));
  propagate_error(check_name(name));

  if (remaining < 2) throw_error("midpoint need 2 points.");

  GeomId xyxy[4];
  propagate_error(object_get_args(POINT, argv[0], xyxy));
  propagate_error(object_get_args(POINT, argv[1], xyxy + 2));

  const GeomId arg_x = graph_add_value(0);
  const GeomId arg_y = graph_add_value(0);
  const GeomId arg_xy[] = {arg_x, arg_y};
  graph_add_constraint(4, xyxy, 2, arg_xy, midpoint);
  object_create(POINT, arg_xy, name, color);
  return 0;
}
