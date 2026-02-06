#ifndef GGB_BOARD_H
#define GGB_BOARD_H

void board_init(float x, float y, float width, float height);
void board_listen();
void board_draw();
void board_cleanup();

void board_update_buffer();

#endif //GGB_BOARD_H