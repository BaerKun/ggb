#include "argparse.h"
#include "message.h"
#include "object.h"
#include <math.h>

static void line_from_2points(const float xyxy[4], float *line[3]) {
  const float x1 = xyxy[0];
  const float y1 = xyxy[1];
  const float x2 = xyxy[2];
  const float y2 = xyxy[3];
  const float dx = x2 - x1;
  const float dy = y2 - y1;
  const float dist = sqrtf(dx * dx + dy * dy);
  if (dist < EPS) return;

  const float nx = -dy / dist;
  const float ny = dx / dist;
  *line[0] = nx;
  *line[1] = ny;
  *line[2] = nx * x1 + ny * y1; // dd = n · (x, y)
}

static void clip_end_point(const float inputs[4], float *t[1]) {
  const float nx = inputs[0];
  const float ny = inputs[1];
  const float px = inputs[2];
  const float py = inputs[3];
  *t[0] = ny * px - nx * py; // (ny, -nx) · (px, py)
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

  int32_t color;
  propagate_error(parse_color(color_str, &color));
  propagate_error(check_new_name(name));

  if (remaining < 2) throw_error("line <point> <point> [--seg]");

  GeomId xyxy[4];
  if (!object_get_args(POINT, argv[0], xyxy)) return MSG_ERROR;
  if (!object_get_args(POINT, argv[1], xyxy + 2)) return MSG_ERROR;

  const GeomId nx = graph_add_value(0);
  const GeomId ny = graph_add_value(0);
  const GeomId dd = graph_add_value(0);
  const GeomId t1 = graph_add_value(-HUGE_VALUE);
  const GeomId t2 = graph_add_value(HUGE_VALUE);
  const GeomId args[] = {nx, ny, dd, t1, t2};
  graph_add_constraint(4, xyxy, 3, args, line_from_2points);
  if (ray || seg) {
    const GeomId inputs[] = {nx, ny, xyxy[0], xyxy[1]};
    graph_add_constraint(4, inputs, 1, &t1, clip_end_point);
  }
  if (seg) {
    const GeomId inputs[] = {nx, ny, xyxy[2], xyxy[3]};
    graph_add_constraint(4, inputs, 1, &t2, clip_end_point);
  }
  object_create(LINE, args, name, color);
  return 0;
}