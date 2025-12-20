#ifndef BOARD_H
#define BOARD_H

#include "object.h"

void board_init();
void board_cleanup();
void board_draw(GeomObject *objs);
void board_refresh();

#endif //BOARD_H
