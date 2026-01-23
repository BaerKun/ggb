#include "object.h"
#include "message.h"
#include "argparse.h"

int cmd_delete(const int argc, const char **argv) {
  static int all;
  static struct argparse parse;
  static struct argparse_option options[] ={
    OPT_BOOLEAN(0, "all", &all)
  };

  all = 0;
  argparse_init(&parse, options, NULL, 0);
  const int remaining = argparse_parse(&parse, argc, argv);

  if (all) {
    object_traverse(object_delete);
    return 0;
  }

  int code = 0;
  for (int i = 0; i < remaining; i++) {
    const GeomObject *obj = object_find(ANY, argv[i]);
    if (obj == NULL) {
      push_error_fmt("object '%s' does not exist.", argv[i]);
      code = MSG_ERROR;
      continue;
    }
    object_delete(obj);
  }

  return code;
}