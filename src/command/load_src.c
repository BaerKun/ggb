#include "commandline.h"
#include <stdio.h>

int load_src(const int argc, const char **argv) {
  static char buffer[CLI_BUF_SIZE];

  for (int i = 1; i < argc; ++i) {
    FILE *file = fopen(argv[i], "r");
    if(file == NULL) {
      fprintf(stderr, "Error: can't open file '%s'.", argv[i]);
      continue;
    }

    while (fgets(buffer, CLI_BUF_SIZE, file)) {
      const char ch = buffer[CLI_BUF_SIZE - 2];
      if (ch != '\0' && ch != '\n') {
        return 1; // Error: command line too long.
      }
      const int code = commandline_parse(buffer);
      if (code) return code;
    }

    fclose(file);
  }
  return 0;
}