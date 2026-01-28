#include "argparse.h"
#include "message.h"
#include "object.h"

static void parallel_eval(const float inputs[4], float *outputs[3]) {
  const float nx = inputs[0];
  const float ny = inputs[1];
  const float px = inputs[2];
  const float py = inputs[3];
  *outputs[0] = nx;
  *outputs[1] = ny;
  *outputs[2] = nx * px + ny * py; // parallel line dd
}

int cmd_parallel(const int argc, const char **argv) {
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

  if (remaining < 2) throw_error("parallel <line> <point>");

  GeomId inputs[5]; // nx, ny, px, py
  if (!object_get_args(LINE, argv[0], inputs)) return MSG_ERROR;
  if (!object_get_args(POINT, argv[1], inputs + 2)) return MSG_ERROR;

  GeomId args[5];
  args[0] = graph_add_value(0);
  args[1] = graph_add_value(0);
  args[2] = graph_add_value(0);
  args[3] = graph_add_value(-HUGE_VALUE);
  args[4] = graph_add_value(HUGE_VALUE);
  graph_add_constraint(4, inputs, 3, args, parallel_eval);
  object_create(LINE, args, name, color);
  return 0;
}