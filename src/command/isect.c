#include "argparse.h"
#include "message.h"
#include "object.h"
#include <math.h>

static void isect_line_line(const float inputs[6], float *outputs[2]) {
  const float nx1 = inputs[0];
  const float ny1 = inputs[1];
  const float dd1 = inputs[2];
  const float nx2 = inputs[3];
  const float ny2 = inputs[4];
  const float dd2 = inputs[5];
  const float D = nx1 * ny2 - nx2 * ny1;
  if (fabsf(D) < EPS) return;
  *outputs[0] = (ny2 * dd1 - ny1 * dd2) / D;
  *outputs[1] = (nx1 * dd2 - nx2 * dd1) / D;
}

static void isect_line_circle(const float inputs[6], float *outputs[4]) {
  const float nx = inputs[0];
  const float ny = inputs[1];
  const float dd = inputs[2];
  const float cx = inputs[3];
  const float cy = inputs[4];
  const float r  = inputs[5];
  const float cross = nx * cy - ny * cx;
  const float tmp = nx * cx + ny * cy - dd;
  const float B = 2 * cross;
  const float C = cross * cross + (tmp + r) * (tmp - r);
  const float delta = B * B - 4 * C;
  if (delta < EPS) return;
  const float sqrt_delta = sqrtf(delta);
  const float t1 = (-B + sqrt_delta) / 2.f;
  const float t2 = (-B - sqrt_delta) / 2.f;
  *outputs[0] = nx * dd + ny * t1;
  *outputs[1] = ny * dd - nx * t1;
  *outputs[2] = nx * dd + ny * t2;
  *outputs[3] = ny * dd - nx * t2;
}

static void isect_circle_circle(const float inputs[6], float *outputs[4]) {
  const float x1 = inputs[0];
  const float y1 = inputs[1];
  const float r1 = inputs[2];
  const float x2 = inputs[3];
  const float y2 = inputs[4];
  const float r2 = inputs[5];
  const float dx = x2 - x1;
  const float dy = y2 - y1;
  const float d = sqrtf(dx * dx + dy * dy);
  if (d > r1 + r2) return;
  const float a = (d * d + r1 * r1 - r2 * r2) / (2 * d);
  const float h = sqrtf(r1 * r1 - a * a);
  const float ux = dx / d;
  const float uy = dy / d;
  const float px = x1 + a * ux;
  const float py = y1 + a * uy;
  *outputs[0] = px + h * uy;
  *outputs[1] = py - h * ux;
  *outputs[2] = px - h * uy;
  *outputs[3] = py + h * ux;
}

int cmd_isect(const int argc, const char **argv) {
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
  propagate_error(check_new_name(name));

  if (remaining < 2) throw_error("isect <line/circle> <line/circle>");

  GeomId inputs[8];
  const ObjectType type1 = object_get_args(LINE | CIRCLE, argv[0], inputs);
  if (type1 == UNKNOWN) return MSG_ERROR;
  const ObjectType type2 = object_get_args(LINE | CIRCLE, argv[1], inputs + 3);
  if (type2 == UNKNOWN) return MSG_ERROR;

  GeomId outputs[4];
  outputs[0] = graph_add_value(0);
  outputs[1] = graph_add_value(0);

  if (type1 == LINE && type2 == LINE) {
    graph_add_constraint(6, inputs, 2, outputs, isect_line_line);
    object_create(POINT, outputs, name, color);
    return 0;
  }

  message_push_back(MSG_WARN, "2 intersections will use the default name.");

  outputs[2] = graph_add_value(0);
  outputs[3] = graph_add_value(0);
  if (type1 == LINE && type2 == CIRCLE) {
    graph_add_constraint(6, inputs, 4, outputs, isect_line_circle);
  } else if (type1 == CIRCLE && type2 == CIRCLE) {
    graph_add_constraint(6, inputs, 4, outputs, isect_circle_circle);
  } else {
    const GeomId swap_inputs[6] = {inputs[3], inputs[4], inputs[5],
                                   inputs[0], inputs[1], inputs[2]};
    graph_add_constraint(6, swap_inputs, 4, outputs, isect_line_circle);
  }
  object_create(POINT, outputs, NULL, color);
  object_create(POINT, outputs + 2, NULL, color);
  return 0;
}