#include "argparse.h"
#include "message.h"
#include "object.h"
#include <math.h>
#include <stdlib.h>

static void radius_from_point(const float xyxy[4], float *radius[1]) {
  const float dx = xyxy[2] - xyxy[0];
  const float dy = xyxy[3] - xyxy[1];
  *radius[0] = sqrtf(dx * dx + dy * dy);
}

int cmd_circle(const int argc, const char **argv) {
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
    throw_error("'circle' need a center and a radius or point on circle.");
  }

  GeomId cr_args[3];
  propagate_error(object_get_args(POINT, argv[0], cr_args));

  const char *str = argv[1];
  if (*str >= '0' && *str <= '9') {
    char *end;
    const float radius = strtof(str, &end);
    if (*end) throw_error_fmt("constant radius need a number. got '%s'.", str);
    cr_args[2] = graph_add_value(radius);
  } else {
    GeomId inputs[4] = {cr_args[0], cr_args[1]};
    propagate_error(object_get_args(POINT, str, inputs + 2));
    cr_args[2] = graph_add_value(0);
    graph_add_constraint(4, inputs, 1, cr_args + 2, radius_from_point);
  }

  object_create(CIRCLE, cr_args, name, color);
  return 0;
}