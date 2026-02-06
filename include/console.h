#ifndef GGB_CONSOLE_H
#define GGB_CONSOLE_H

#include <stdbool.h>

void console_init(float x, float y, float width, float height);
void console_listen();
void console_draw();
void console_cleanup();

void console_push_text(const char* text, bool select);

#endif //GGB_CONSOLE_H