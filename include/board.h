#ifndef GGB_BOARD_H
#define GGB_BOARD_H

#include "types.h"

typedef enum {
  MOUSE_PRESS = 1,
  MOUSE_RELEASE = 2
} MouseEvent;

typedef void (*BoardControl)(Vec2 pos, MouseEvent event);

void board_init(float x, float y, float w, float h);
void board_listen();
void board_draw();
void board_cleanup();

void board_update_buffer();
void board_set_control(BoardControl ctrl);

Vec2 xform_to_world(Vec2 pos);
GeomId board_select_object(ObjectType types, Vec2 pos);

#endif // GGB_BOARD_H