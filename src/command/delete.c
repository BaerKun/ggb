#include "object.h"
#include "message.h"
#include "argparse.h"

int delete_(const int argc, const char **argv) {
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

  for (int i = 0; i < remaining; i++) {
    const GeomObject *obj = object_find(ANY, argv[i]);
    if (obj == NULL) {
      throw_error_fmt(OBJECT_NOT_EXISTS, "object '%s' does not exist.",
                      argv[i]);
    }
    object_delete(obj);
  }
  return 0;
}