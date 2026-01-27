#ifndef GGB_COMMAND_H
#define GGB_COMMAND_H

int cmd_point(int, const char **);
int cmd_line(int, const char **);
int cmd_circle(int, const char **);

int cmd_midpoint(int, const char **);
int cmd_perp(int, const char **);
int cmd_parallel(int, const char **);
int cmd_bisector(int, const char **);

int cmd_delete(int, const char **);
int cmd_move_pt(int, const char **);

int cmd_load_src(int, const char **);

#endif // GGB_COMMAND_H