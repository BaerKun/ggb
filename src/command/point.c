#include "argparse.h"
#include "message.h"
#include "object.h"
#include <stdlib.h>

int cmd_point(const int argc, const char **argv) {
  static char *name, *color_str;
  static int group;
  static struct argparse parse;
  static struct argparse_option opt[] = {
      OPT_STRING('n', "name", &name), OPT_STRING('c', "color", &color_str),
      OPT_INTEGER('g', "group", &group), OPT_END()};

  name = color_str = NULL, group = 0;
  argparse_init(&parse, opt, NULL, 0);
  const int remaining = argparse_parse(&parse, argc, argv);
  if (remaining < 0) return MSG_ERROR;

  int32_t color;
  propagate_error(parse_new_name(name, 1, &name));
  propagate_error(parse_color(color_str, &color));
  propagate_error(check_group(group));

  if (remaining == 0) throw_error("point <xy:(%%f,%%f)>");

  GeomId args[2];
  const char *str = argv[0];
  if (*str == '(') {
    static const char error_fmt[] =
        "'%s' isn't a valid coordinate. must be '(%%f,%%f)'";
    char *end;
    const char *start = str + 1;
    const float x = strtof(start, &end);
    if (end == start || *end != ',') throw_error_fmt(error_fmt, str);
    start = end + 1;
    const float y = strtof(start, &end);
    if (end == start || *end != ')' || end[1]) throw_error_fmt(error_fmt, str);
    args[0] = graph_add_value(x);
    args[1] = graph_add_value(y);
  } else {
    if (!object_get_args(POINT, str, args)) return MSG_ERROR;
  }

  object_create(POINT, args, name, group, color);
  return 0;
}