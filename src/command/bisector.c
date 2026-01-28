#include "argparse.h"
#include "message.h"
#include "object.h"
#include <math.h>
#include <string.h>

enum { CW = 1, CCW = 2, BOTH = 3 };

static void angle_bisector_ccw(const float inputs[6], float *outputs[3]) {
  const float nx1 = inputs[0];
  const float ny1 = inputs[1];
  const float dd1 = inputs[2];
  const float nx2 = inputs[3];
  const float ny2 = inputs[4];
  const float dd2 = inputs[5];
  const float cross = nx1 * ny2 - ny1 * nx2;
  const float nx = cross > 0 ? nx1 + nx2 : nx1 - nx2;
  const float ny = cross > 0 ? ny1 + ny2 : ny1 - ny2;
  const float dd = cross > 0 ? dd1 + dd2 : dd1 - dd2;
  const float norm = sqrtf(nx * nx + ny * ny);
  *outputs[0] = nx / norm;
  *outputs[1] = ny / norm;
  *outputs[2] = dd / norm;
}

static void create_angle_bisector_ccw(const GeomId inputs[6], const char *name,
                                      const int32_t color) {
  GeomId args[5];
  args[0] = graph_add_value(0);
  args[1] = graph_add_value(0);
  args[2] = graph_add_value(0);
  args[3] = graph_add_value(-HUGE_VALUE);
  args[4] = graph_add_value(HUGE_VALUE);
  graph_add_constraint(6, inputs, 3, args, angle_bisector_ccw);
  object_create(LINE, args, name, color);
}

int cmd_bisector(const int argc, const char **argv) {
  static char *name, *color_str, *wise;
  static struct argparse parse;
  static struct argparse_option options[] = {
      OPT_STRING('n', "name", &name), OPT_STRING('c', "color", &color_str),
      OPT_STRING(0, "wise", &wise), OPT_END()};

  name = color_str = wise = NULL;
  argparse_init(&parse, options, NULL, 0);
  const int remaining = argparse_parse(&parse, argc, argv);
  if (remaining < 0) return MSG_ERROR;

  int32_t color;
  propagate_error(check_new_name(name));
  propagate_error(parse_color(color_str, &color));

  int mode;
  if (wise == NULL) {
    mode = BOTH;
    if (name != NULL) {
      name = NULL;
      message_push_back(MSG_WARN,
                        "2 angle bisectors will use the default name.");
    }
  } else if (strcmp(wise, "clock") == 0) {
    mode = CW;
  } else if (strcmp(wise, "counter") == 0) {
    mode = CCW;
  } else {
    throw_error("'wise' option should be 'clock' or 'counter'.");
  }

  if (remaining == 0) {
    throw_error("bisector <line> <line> [--wise=clock/counter]");
  }

  GeomId inputs[8]; // nx1, ny1, dd1, nx1, nx2, dd2
  if (!object_get_args(LINE, argv[0], inputs)) return MSG_ERROR;
  if (!object_get_args(LINE, argv[1], inputs + 3)) return MSG_ERROR;

  if (mode & CCW) {
    create_angle_bisector_ccw(inputs, name, color);
  }
  if (mode & CW) {
    const GeomId inputs_cw[6] = {inputs[3], inputs[4], inputs[5],
                                 inputs[0], inputs[1], inputs[2]};
    create_angle_bisector_ccw(inputs_cw, name, color);
  }
  return 0;
}