#ifndef GGB_BOARD_H
#define GGB_BOARD_H

#include "types.h"

typedef void (*BoardControl)(Vec2 pos, bool click);

void board_init(float x, float y, float w, float h);
void board_listen();
void board_draw();
void board_cleanup();

Vec2 xform_to_world(Vec2 pos);
GeomId board_select_object(ObjectType types, Vec2 pos);
void board_update_buffer();

#endif //GGB_BOARD_H