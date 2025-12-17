#include "commandline.h"
#include "str_hash.h"

#define CMD_NUM 16
#define ARGV_SIZE 24

static StringHashTable cmd_hash = static_string_hash_table(CMD_NUM * 2);

static void commandline_init() {
  string_hash_table_insert(&cmd_hash, "create");
  string_hash_table_insert(&cmd_hash, "show");
  string_hash_table_insert(&cmd_hash, "hide");
  string_hash_table_insert(&cmd_hash, "move-pt");
  string_hash_table_insert(&cmd_hash, "load-src");
  string_hash_table_insert(&cmd_hash, "midpoint");
}

int read_line(FILE *stream, char *buffer) {
  if (!fgets(buffer, CMD_BUFF_SIZE, stream)) {
    return 1;
  }

  const char ch = buffer[CMD_BUFF_SIZE - 2];
  if (ch != '\0' && ch != '\n') {
    return 2;
  }
  return 0;
}

static int split_args(char *ptr, const char **argv) {
  int argc = 0, first_letter = 1;

  while (1) {
    if (*ptr == '\n') {
      *ptr = '\0';
      return argc;
    } else if (*ptr == ' ') {
      *ptr = '\0';
      first_letter = 1;
    } else if (first_letter) {
      first_letter = 0;
      argv[argc] = ptr;
      if (++argc == ARGV_SIZE) return ARGV_SIZE;
    }
  }
}

int commandline_parse(char *line) {
  static int init = 1;
  if (init) commandline_init(), init = 0;

  static const char *argv[ARGV_SIZE];
  const int argc = split_args(line, argv);
  const char *cmd = argv[0];

}