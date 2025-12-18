#include "commandline.h"
#include "str_hash.h"

#define DECLARE_COMMAND(cmd) int cmd(int, const char **);
DECLARE_COMMAND(create);
DECLARE_COMMAND(show);
DECLARE_COMMAND(hide);
DECLARE_COMMAND(move_pt);
DECLARE_COMMAND(load_src);
DECLARE_COMMAND(midpoint);

typedef int (*CommandFunction)(int, const char **);

typedef struct {
  const char *name;
  CommandFunction func;
} CommandMapEntry;

CommandMapEntry cmd_map[] = {
    {"create", create},
    {"show", show},
    {"hide", hide},
    {"move-pt", move_pt},
    {"load-src", load_src},
    {"midpoint", midpoint}
};

#define CMD_NUM (sizeof(cmd_map) / sizeof(CommandMapEntry))
#define ARGV_SIZE 24

static StringHashTable cmd_hash;
static CommandFunction cmd_func[CMD_NUM];

void commandline_init() {
  string_hash_init(&cmd_hash, CMD_NUM);
  for (int i = 0; i < CMD_NUM; i++) {
    cmd_func[string_hash_insert(&cmd_hash, cmd_map[i].name)] = cmd_map[i].func;
  }
}

void commandline_cleanup() {
  string_hash_free(&cmd_hash);
}

int read_line(FILE *stream, char *buffer) {
  buffer[CMD_BUFF_SIZE - 2] = '\0';

  if (!fgets(buffer, CMD_BUFF_SIZE, stream)) {
    // Error:
    return 1;
  }

  const char ch = buffer[CMD_BUFF_SIZE - 2];
  if (ch != '\0' && ch != '\n') {
    // Error: command line too long.
    return 2;
  }

  return 0;
}

static int split_args(char *ptr, const char **argv) {
  int argc = 0, first_letter = 1;
  for (;; ++ptr) {
    switch (*ptr) {
    case '\n':
    case '\t':
      // case '\r':
      *ptr = '\0';
    case '\0':
      return argc;
    case ' ':
      *ptr = '\0';
      first_letter = 1;
      break;
    default:
      if (first_letter) {
        first_letter = 0;
        argv[argc] = ptr;
        if (++argc == ARGV_SIZE) return ARGV_SIZE;
      }
    }
  }
}

int commandline_parse(char *line) {
  static const char *argv[ARGV_SIZE];
  const int argc = split_args(line, argv);
  const char *cmd = argv[0];
  const int cmd_id = string_hash_find(&cmd_hash, cmd);
  if (cmd_id == -1) {
    fprintf(stderr, "Error: command not found.\n");
    return 0;
  }
  return cmd_func[cmd_id](argc, argv);
}