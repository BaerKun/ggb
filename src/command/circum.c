#include "argparse.h"
#include "message.h"
#include "object.h"

#include <math.h>

static void circumcircle_eval(const float inputs[6], float *outputs[3]) {
  const float x1 = inputs[0], y1 = inputs[1];
  const float x2 = inputs[2], y2 = inputs[3];
  const float x3 = inputs[4], y3 = inputs[5];
  const float D = 2 * (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2));
  if (fabsf(D) < EPS) return;

  const float sq_xy1 = x1 * x1 + y1 * y1;
  const float sq_xy2 = x2 * x2 + y2 * y2;
  const float sq_xy3 = x3 * x3 + y3 * y3;
  const float cx =
      (sq_xy1 * (y2 - y3) + sq_xy2 * (y3 - y1) + sq_xy3 * (y1 - y2)) / D;
  const float cy =
      (sq_xy1 * (x3 - x2) + sq_xy2 * (x1 - x3) + sq_xy3 * (x2 - x1)) / D;
  const float rx = x1 - cx;
  const float ry = y1 - cy;
  *outputs[0] = cx;
  *outputs[1] = cy;
  *outputs[2] = sqrtf(rx * rx + ry * ry);
}

int cmd_circum(const int argc, const char **argv) {
  static char *name, *color_str;
  static struct argparse parse;
  static struct argparse_option options[] = {
      OPT_STRING('n', "name", &name), OPT_STRING('c', "color", &color_str),
      OPT_END()};

  name = color_str = NULL;
  argparse_init(&parse, options, NULL, 0);
  const int remaining = argparse_parse(&parse, argc, argv);
  if (remaining < 0) return MSG_ERROR;

  int32_t color;
  propagate_error(check_new_name(name));
  propagate_error(parse_color(color_str, &color));

  if (remaining < 3) throw_error("circum <point> <point> <point>");

  GeomId inputs[6];
  if(!object_get_args(POINT, argv[0], inputs)) return MSG_ERROR;
  if(!object_get_args(POINT, argv[1], inputs + 2)) return MSG_ERROR;
  if(!object_get_args(POINT, argv[2], inputs + 4)) return MSG_ERROR;

  GeomId args[3];
  args[0] = graph_add_value(0);
  args[1] = graph_add_value(0);
  args[2] = graph_add_value(0);
  graph_add_constraint(6, inputs, 3, args, circumcircle_eval);
  object_create(CIRCLE, args, name, color);
  return 0;
}