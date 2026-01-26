#include "argparse.h"
#include "message.h"
#include "object.h"

static void perp_eval(const float inputs[4], float *output[3]) {
  const float px = inputs[0];
  const float py = inputs[1];
  const float nx = inputs[2];
  const float ny = inputs[3];
  *output[0] = ny;
  *output[1] = -nx;
  *output[2] = px * ny - nx * py;
}

static void perp_foot_eval(const float inputs[4], float *output[2]) {
  const float nx = inputs[0]; // perp line
  const float ny = inputs[1];
  const float d1 = inputs[2]; // perp line
  const float d2 = inputs[3]; // baseline
  *output[0] = nx * d1 - ny * d2;
  *output[1] = ny * d1 + nx * d2;
}

static void clip_end_point(const float inputs[4], float *t[1]) {
  const float nx = inputs[0];
  const float ny = inputs[1];
  const float px = inputs[2];
  const float py = inputs[3];
  *t[0] = px * ny - nx * py;
}

int cmd_perp(const int argc, const char **argv) {
  static char *name, *color_str;
  static int seg;
  static struct argparse parse;
  static struct argparse_option opt[] = {
      OPT_STRING('n', "name", &name), OPT_STRING('c', "color", &color_str),
      OPT_BOOLEAN(0, "seg", &seg), OPT_END()};

  name = color_str = NULL, seg = 0;
  argparse_init(&parse, opt, NULL, 0);
  const int remaining = argparse_parse(&parse, argc, argv);
  if (remaining < 0) return MSG_ERROR;

  int32_t color;
  propagate_error(check_name(name));
  propagate_error(parse_color(color_str, &color));

  if (remaining == 0) throw_error("perp <point> <line> [--seg]");

  GeomId perp_inputs[7]; // px, py, nx, ny, dd
  propagate_error(object_get_args(POINT, argv[0], perp_inputs));
  propagate_error(object_get_args(LINE, argv[1], perp_inputs + 2));

  GeomId perp_args[5];
  perp_args[0] = graph_add_value(0);
  perp_args[1] = graph_add_value(0);
  perp_args[2] = graph_add_value(0);
  perp_args[3] = graph_add_value(-HUGE_VALUE);
  perp_args[4] = graph_add_value(HUGE_VALUE);
  graph_add_constraint(4, perp_inputs, 3, perp_args, perp_eval);
  object_create(LINE, perp_args, name, color);

  GeomId foot_args[2];
  foot_args[0] = graph_add_value(0);
  foot_args[1] = graph_add_value(0);
  const GeomId foot_inputs[4] = {perp_args[0], perp_args[1], perp_args[2], perp_inputs[4]};
  graph_add_constraint(4, foot_inputs, 2, foot_args, perp_foot_eval);
  object_create(POINT, foot_args, name, color);

  if (seg) {
    GeomId inputs[] = {perp_args[0], perp_args[1], perp_inputs[0], perp_inputs[1]};
    graph_add_constraint(4, inputs, 1, perp_args + 3, clip_end_point);
    inputs[2] = foot_args[0];
    inputs[3] = foot_args[1];
    graph_add_constraint(4, inputs, 1, perp_args + 4, clip_end_point);
  }
  return 0;
}