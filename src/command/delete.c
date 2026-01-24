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
    object_delete_all();
    return 0;
  }

  for (int i = 0; i < remaining; i++) {
    propagate_error(object_delete(argv[i]));
  }

  return 0;
}