#include "commandline.h"
#include "str_hash.h"
#include "command.h"
#include "message.h"

#define ARGV_SIZE 24

typedef int (*CommandFunction)(int, const char **);

typedef struct {
  const char *name;
  CommandFunction func;
} CommandMapEntry;

CommandMapEntry cmd_map[] = {{"point", cmd_point},
                             {"line", cmd_line},
                             {"circle", cmd_circle},
                             {"delete", cmd_delete},
                             {"move-pt", cmd_move_pt},
                             {"load-src", cmd_load_src},
                             {"midpoint", cmd_midpoint}};

const int CMD_NUM = (sizeof(cmd_map) / sizeof(CommandMapEntry));
static StringHashTable cmd_hash;

void commandline_init() {
  string_hash_init(&cmd_hash, CMD_NUM);
  for (int i = 0; i < CMD_NUM; i++) {
    const int id = string_hash_alloc_id(&cmd_hash);
    string_hash_insert(&cmd_hash, cmd_map[id].name, id);
  }
}

void commandline_cleanup() { string_hash_free(&cmd_hash); }

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
  if (argc == 0) return 0;

  const char *cmd = argv[0];
  const int cmd_id = string_hash_find(&cmd_hash, cmd);
  if (cmd_id == -1) {
    throw_error_fmt("unknown command '%s'", cmd);
  }
  return cmd_map[cmd_id].func(argc, argv);
}