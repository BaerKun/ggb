#include "message.h"
#include "object.h"
#include <stdio.h>
#include <string.h>

#define MAX_POINT_COUNT 10

int cmd_move_pt(int argc, const char **argv) {
  static const char help[] = "move-pt <from...> to <to...>.";
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
    propagate_error(object_get_args(POINT, argv[i], src + i * 2));
  }

  const char **arg_dst = argv + count + 1;
  for (int i = 0; i < count; i++) {
    const char *str = arg_dst[i];
    if (sscanf(str, "%f,%f", dst + i * 2, dst + i * 2 + 1) != 2) {
      throw_error_fmt("'%s' isn't a valid coordinate. must be '%%f,%%f'", str);
    }
  }

  graph_change_value(count * 2, src, dst);
  return 0;
}