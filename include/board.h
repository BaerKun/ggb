#ifndef GGB_BOARD_H
#define GGB_BOARD_H

void board_init(float x, float y, float width, float height);
void board_draw_update();
void board_update_buffer();
void board_cleanup();

#endif //GGB_BOARD_H