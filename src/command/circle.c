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
  static int group;
  static struct argparse parse;
  static struct argparse_option opt[] = {
      OPT_STRING('n', "name", &name), OPT_STRING('c', "color", &color_str),
      OPT_INTEGER('g', "group", &group), OPT_END()};

  name = color_str = NULL, group = 0;
  argparse_init(&parse, opt, NULL, 0);
  const int remaining = argparse_parse(&parse, argc, argv);
  if (remaining < 0) return MSG_ERROR;

  int32_t color;
  propagate_error(parse_new_name(name, 1, &name));
  propagate_error(parse_color(color_str, &color));
  propagate_error(check_group(group));

  if (remaining < 2) {
    throw_error("circle <center> <radius:number / point:on circle>");
  }

  GeomId center[2];
  if (!object_get_args(POINT, argv[0], center)) return MSG_ERROR;

  GeomId radius;
  const char *str = argv[1];
  if (*str >= '0' && *str <= '9') {
    char *end;
    const float value = strtof(str, &end);
    if (*end) throw_error_fmt("constant radius need a number. got '%s'.", str);
    radius = graph_add_value(value);
  } else {
    GeomId inputs[4] = {center[0], center[1]};
    if(!object_get_args(POINT, str, inputs + 2)) return MSG_ERROR;
    radius = graph_add_value(0);
    graph_add_constraint(4, inputs, 1, &radius, radius_from_point);
  }

  const GeomId args[] = {center[0], center[1], radius};
  object_create(CIRCLE, args, name, group, color);
  return 0;
}