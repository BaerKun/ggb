#include "argparse.h"
#include "message.h"
#include "object.h"

static void perp_eval(const float inputs[4], float *output[3]) {
  const float nx = inputs[0];
  const float ny = inputs[1];
  const float px = inputs[2];
  const float py = inputs[3];
  *output[0] = -ny;
  *output[1] = nx;
  *output[2] = -ny * px + nx * py; // np · (px, py)
}

static void perp_foot_eval(const float inputs[4], float *output[2]) {
  const float nx = inputs[0]; // perp line
  const float ny = inputs[1];
  const float d1 = inputs[2]; // perp line
  const float d2 = inputs[3]; // baseline
  *output[0] = nx * d1 + ny * d2;
  *output[1] = ny * d1 - nx * d2;
}

static void clip_end_point(const float inputs[4], float *t[1]) {
  const float nx = inputs[0];
  const float ny = inputs[1];
  const float px = inputs[2];
  const float py = inputs[3];
  *t[0] = ny * px - nx * py;
}

static void equal(const float input[1], float *output[1]) {
  *output[0] = input[0];
}

int cmd_perp(const int argc, const char **argv) {
  static char *name_str, *color_str;
  static int group, seg, foot;
  static struct argparse parse;
  static struct argparse_option opt[] = {
      OPT_STRING('n', "name", &name_str), OPT_STRING('c', "color", &color_str),
      OPT_INTEGER('g', "group", &group),  OPT_BOOLEAN(0, "seg", &seg),
      OPT_BOOLEAN(0, "foot", &foot),      OPT_END()};

  name_str = color_str = NULL, group = seg = foot = 0;
  argparse_init(&parse, opt, NULL, 0);
  const int remaining = argparse_parse(&parse, argc, argv);
  if (remaining < 0) return MSG_ERROR;

  char *names[2];
  int32_t color;
  propagate_error(parse_new_name(name_str, 2, names));
  propagate_error(parse_color(color_str, &color));
  propagate_error(check_group(group));

  if (remaining == 0) throw_error("perp <line> <point> [--seg]");

  GeomId line_args[5], point_args[2];
  if (!object_get_args(LINE, argv[0], line_args)) return MSG_ERROR;
  if (!object_get_args(POINT, argv[1], point_args)) return MSG_ERROR;

  const GeomId perp_inputs[] = {line_args[0], line_args[1], point_args[0],
                                point_args[1]};
  GeomId perp_args[5];
  perp_args[0] = graph_add_value(0);
  perp_args[1] = graph_add_value(0);
  perp_args[2] = graph_add_value(0);
  perp_args[3] = graph_add_value(-HUGE_VALUE);
  perp_args[4] = graph_add_value(HUGE_VALUE);
  graph_add_constraint(4, perp_inputs, 3, perp_args, perp_eval);
  object_create(LINE, perp_args, names[0], group, color);

  if (foot) {
    GeomId foot_args[2];
    foot_args[0] = graph_add_value(0);
    foot_args[1] = graph_add_value(0);
    const GeomId foot_inputs[4] = {perp_args[0], perp_args[1], perp_args[2],
                                   line_args[2]};
    graph_add_constraint(4, foot_inputs, 2, foot_args, perp_foot_eval);
    object_create(POINT, foot_args, names[1], group, -1);
  }

  if (seg) {
    const GeomId inputs[] = {perp_args[0], perp_args[1], point_args[0],
                             point_args[1]};
    graph_add_constraint(4, inputs, 1, perp_args + 3, clip_end_point);

    // foot-t = baseline-dd
    graph_add_constraint(1, line_args + 2, 1, perp_args + 4, equal);
  }
  return 0;
}