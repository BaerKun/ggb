#include "message.h"
#include "object.h"
#include <stdlib.h>
#include <string.h>

#define MAX_POINT_COUNT 10

static int get_xy(const char *str, float xy[2]) {
  static const char error_fmt[] =
      "'%s' isn't a valid coordinate. must be '(%%f,%%f)'";
  if (*str == '(') {
    char *end;
    const char *start = str + 1;
    xy[0] = strtof(start, &end);
    if (end == start || *end != ',') throw_error_fmt(error_fmt, str);
    start = end;
    xy[0] = strtof(start, &end);
    if (end == start || *end != ')' || end[1]) throw_error_fmt(error_fmt, str);
    return 0;
  }

  GeomId ids[2];
  if (!object_get_args(POINT, str, ids)) return MSG_ERROR;
  xy[0] = graph_get_value(ids[0]);
  xy[1] = graph_get_value(ids[1]);
  return 0;
}

int cmd_move_pt(int argc, const char **argv) {
  static const char help[] = "move-pt <from...> to <to...>";
  argc--;
  argv++;
  if (argc == 0) throw_error(help);

  GeomId src[MAX_POINT_COUNT * 2];
  float dst[MAX_POINT_COUNT * 2];

  GeomSize count = 0;
  for (; count < argc; count++) {
    if (strcmp(argv[count], "to") == 0) break;
  }

  if (count == argc) throw_error(help);
  if (count != argc - count - 1) throw_error("from and to count not equal.");

  for (GeomSize i = 0; i < count; i++) {
    if (!object_get_args(POINT, argv[i], src + i * 2)) return MSG_ERROR;
  }

  const char **arg_dst = argv + count + 1;
  for (int i = 0; i < count; i++) {
    propagate_error(get_xy(arg_dst[i], dst + i * 2));
  }

  graph_change_value(count * 2, src, dst);
  return 0;
}