#ifndef GGB_COMMAND_H
#define GGB_COMMAND_H

#include "board.h"

typedef struct {
  BoardControl ctrl;
  const char *usage;
} Command;

void cmd_point(Command *);
void cmd_line(Command *);
void cmd_circle(Command *);
void cmd_midpoint(Command *);
void cmd_perp(Command *);
void cmd_parallel(Command *);
void cmd_bisector(Command *);
void cmd_circum(Command *);
void cmd_isect(Command *);
void cmd_delete(Command *);
void cmd_move_pt(Command *);
void cmd_load_src(Command *);

#endif // GGB_COMMAND_H
