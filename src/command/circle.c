#include "argparse.h"
#include "message.h"
#include "object.h"
#include <math.h>
#include <stdlib.h>

static inline float vec2_dist(const Vec2 v1, const Vec2 v2) {
  const float dx = v1.x - v2.x;
  const float dy = v1.y - v2.y;
  return sqrtf(dx * dx + dy * dy);
}

static Vec2 by_const_radius(GeomInt argc, const Vec2 *argv) {
  return (Vec2){argv[0].x + argv[1].x, argv[0].y};
}

static Vec2 by_segment_radius(GeomInt argc, const Vec2 *argv) {
  return (Vec2){argv[0].x + vec2_dist(argv[1], argv[2]), argv[0].y};
}

static int by_radius(const char *arg, const GeomId center, GeomId *pt) {
  if (*arg >= '0' && *arg <= '9') {
    char *end;
    const float radius = strtof(arg, &end);
    if (*end) {
      throw_error_fmt("constant radius need a number. got '%s'.", arg);
    }

    const GeomId helper = point_create((Vec2){radius}, FREE);
    GeomId pts[] = {center, helper};
    *pt = point_create(ZERO_POINT, (Constraint){2, pts, by_const_radius});
  } else {
    GeomId seg_p1, seg_p2;
    propagate_error(object_get_points(SEG, arg, &seg_p1, &seg_p2));
    GeomId pts[] = {center, seg_p1, seg_p2};
    *pt = point_create(ZERO_POINT, (Constraint){3, pts, by_segment_radius});
  }
  return 0;
}

int cmd_circle(const int argc, const char **argv) {
  static char *name;
  static int color, as_radius;
  static struct argparse parse;
  static struct argparse_option opt[] = {
      OPT_STRING('n', "name", &name), OPT_INTEGER('c', "color", &color),
      OPT_BOOLEAN(0, "as-radius", &as_radius), OPT_END()};

  name = NULL, color = -1, as_radius = 0;
  argparse_init(&parse, opt, NULL, 0);
  const int remaining = argparse_parse(&parse, argc, argv);
  if (remaining < 0) return MSG_ERROR;

  propagate_error(check_name(name));

  if (remaining < 2) {
    throw_error("'circle' need a center and a point on circle\n"
                "           (or radius when use '--as-radius').");
  }

  GeomId center, pt;
  propagate_error(object_get_points(POINT, argv[0], &center, NULL));

  if (as_radius) {
    propagate_error(by_radius(argv[1], center, &pt));
  } else {
    propagate_error(object_get_points(POINT, argv[1], &pt, NULL));
  }

  object_create(CIRCLE, center, pt, name, DEFAULT_COLOR);
  return 0;
}