#ifndef GGB_COMMAND_H
#define GGB_COMMAND_H

int create(int argc, const char **argv);
int delete_(int argc, const char **argv);
int show(int argc, const char **argv);
int hide(int argc, const char **argv);
int move_pt(int argc, const char **argv);
int load_src(int argc, const char **argv);
int midpoint(int argc, const char **argv);

#endif //GGB_COMMAND_H