#include "commandline.h"

int load_src(const int argc, const char **argv) {
  static char buffer[CLI_BUF_SIZE];

  for (int i = 1; i < argc; ++i) {
    FILE *file = fopen(argv[i], "r");
    if(file == NULL) {
      fprintf(stderr, "Error: can't open file '%s'.", argv[i]);
      continue;
    }

    int err;
    while(!read_line(file, buffer)) {
      if((err = commandline_parse(buffer))) {
        return err;
      }
    }

    fclose(file);
  }
  return 0;
}