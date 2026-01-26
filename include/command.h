#ifndef GGB_COMMAND_H
#define GGB_COMMAND_H

int cmd_point(int argc, const char **argv);
int cmd_line(int argc, const char **argv);
int cmd_circle(int argc, const char **argv);

int cmd_midpoint(int argc, const char **argv);
int cmd_perp(int argc, const char **argv);

int cmd_delete(int argc, const char **argv);
int cmd_move_pt(int argc, const char **argv);

int cmd_load_src(int argc, const char **argv);


#endif //GGB_COMMAND_H