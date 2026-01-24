#include "commandline.h"
#include "message.h"
#include <stdio.h>
#include <string.h>

int cmd_load_src(const int argc, const char **argv) {
  static char buffer[CLI_BUF_SIZE];

  if (argc == 1) throw_error("no file was given.");

  FILE *file = fopen(argv[1], "r");
  if (file == NULL) throw_error_fmt("can't open file '%s'.", argv[1]);

  int line = 0;
  while (fgets(buffer, CLI_BUF_SIZE, file)) {
    line++;
    const char ch = buffer[CLI_BUF_SIZE - 2];
    if (ch != '\0' && ch != '\n') {
      fclose(file);
      throw_error_fmt("line %d: commandline too long.", line);
    }
    const int code = commandline_parse(buffer);
    if (code == MSG_EXIT) {
      fclose(file);
      return MSG_EXIT;
    }
    if (code != MSG_NULL) {
      // silence info/warning
      const Message *msg = message_pop();
      if (code == MSG_ERROR) {
        fclose(file);
        strcpy(buffer, msg->content);
        message_push_front(MSG_ERROR, "line %d: %s", line, buffer);
        return MSG_ERROR;
      }
    }
  }

  fclose(file);
  return 0;
}