#include "commandline.h"
#include "message.h"
#include <stdio.h>

int load_src(const int argc, const char **argv) {
  static char buffer[CLI_BUF_SIZE];

  if (argc == 1) {
    throw_error(MISS_PARAMETER, "no file was given.");
  }

  FILE *file = fopen(argv[1], "r");
  if (file == NULL) {
    throw_error_fmt(FILE_CANT_OPEN, "can't open file '%s'.", argv[1]);
  }

  int line = 0;
  while (fgets(buffer, CLI_BUF_SIZE, file)) {
    line++;
    const char ch = buffer[CLI_BUF_SIZE - 2];
    if (ch != '\0' && ch != '\n') {
      fclose(file);
      throw_error_fmt(COMMANDLINE_TOO_LONG, "line %d: commandline too long.", line);
    }
    const int code = commandline_parse(buffer);
    if (code == MSG_EXIT) {
      fclose(file);
      return MSG_EXIT;
    }
    if (code != MSG_NULL) {
      // silence info/warning
      const Message *msg = message_pop();
      if (code & MSG_ERROR) {
        fclose(file);
        throw_error_fmt(msg->type, "line %d: %s", line, msg->content);
      }
    }
  }

  fclose(file);
  return 0;
}