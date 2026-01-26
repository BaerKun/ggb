#include "argparse.h"
#include "message.h"
#include "object.h"
#include <math.h>

static void line_from_2points(const float xyxy[4], float *line[3]) {
  const float dx = xyxy[2] - xyxy[0];
  const float dy = xyxy[3] - xyxy[1];
  const float dist = sqrtf(dx * dx + dy * dy);
  *line[0] = -dy / dist;
  *line[1] = dx / dist;
  *line[2] = (xyxy[1] * xyxy[2] - xyxy[0] * xyxy[3]) / dist; // directed
}

static void clip_end_point(const float inputs[4], float *t[1]) {
  *t[0] = inputs[1] * inputs[2] - inputs[0] * inputs[3]; // x*ny - y*nx
}

int cmd_line(const int argc, const char **argv) {
  static char *name, *color_str;
  static int ray, seg;
  static struct argparse parse;
  static struct argparse_option opt[] = {
      OPT_STRING('n', "name", &name), OPT_STRING('c', "color", &color_str),
      OPT_BOOLEAN(0, "ray", &ray), OPT_BOOLEAN(0, "seg", &seg), OPT_END()};

  name = color_str = NULL, ray = seg = 0;
  argparse_init(&parse, opt, NULL, 0);
  const int remaining = argparse_parse(&parse, argc, argv);
  if (remaining < 0) return MSG_ERROR;

  Color color;
  propagate_error(parse_color(color_str, &color));
  propagate_error(check_name(name));

  if (remaining < 2) {
    throw_error("'line' need 2 points. [--seg]");
  }

  GeomId xyxy[4];
  propagate_error(object_get_args(POINT, argv[0], xyxy));
  propagate_error(object_get_args(POINT, argv[1], xyxy + 2));

  const GeomId nx = graph_add_value(0);
  const GeomId ny = graph_add_value(0);
  const GeomId dd = graph_add_value(0);
  const GeomId t1 = graph_add_value(-HUGE_VALUE);
  const GeomId t2 = graph_add_value(HUGE_VALUE);
  const GeomId line_args[] = {nx, ny, dd, t1, t2};
  graph_add_constraint(4, xyxy, 3, line_args, line_from_2points);
  if (ray || seg) {
    const GeomId inputs[] = {nx, ny, xyxy[0], xyxy[1]};
    graph_add_constraint(4, inputs, 1, &t1, clip_end_point);
  }
  if (seg) {
    const GeomId inputs[] = {nx, ny, xyxy[2], xyxy[3]};
    graph_add_constraint(4, inputs, 1, &t2, clip_end_point);
  }
  object_create(LINE, line_args, name, color);
  return 0;
}