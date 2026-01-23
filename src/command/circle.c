#include "argparse.h"
#include "message.h"
#include "object.h"
#include <stdlib.h>

static Vec2 by_const_radius(GeomInt argc, const Vec2 *argv) {
  return (Vec2){argv[0].x + argv[1].x, argv[0].y};
}

static Vec2 by_segment_radius(GeomInt argc, const Vec2 *argv) {
  return (Vec2){argv[0].x + vec2_distance(argv[1], argv[2]), argv[0].y};
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

  const GeomObject *obj = object_find(POINT, argv[0]);
  if (obj == NULL) {
    throw_error_fmt("point '%s' doesn't exist.", argv[0]);
  }
  const GeomId center = obj->pt1;

  GeomId pt;
  const char *arg = argv[1];
  if (as_radius) {
    if (*arg >= '0' && *arg <= '9') {
      char *end;
      const float radius = strtof(arg, &end);
      if (*end) {
        throw_error_fmt("constant radius need a number. got '%s'.", arg);
      }
      const GeomId helper = point_create((Vec2){radius}, FREE);
      GeomId pts[] = {obj->pt1, helper};
      pt = point_create(ZERO_POINT, (Constraint){2, pts, by_const_radius});
    } else {
      const GeomObject *seg = object_find(SEG, arg);
      if (seg == NULL) {
        throw_error_fmt("line segment '%s' doesn't exist.", arg);
      }
      GeomId pts[] = {center, seg->pt1, seg->pt2};
      pt = point_create(ZERO_POINT, (Constraint){2, pts, by_const_radius});
    }
  } else {
    const GeomObject *pt_obj = object_find(POINT, arg);
    if (pt_obj == NULL) {
      throw_error_fmt("point '%s' doesn't exist.", arg);
    }
    pt = pt_obj->pt1;
  }

  object_create(CIRCLE, center, pt, name, color);
  return 0;
}