#include "graphical.h"
#include "geometry.h"
#include "console.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

Window *mainWindow, *imageWindow;
Point2i origin = {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2};

int main() {
  graphicalInit();

  mainWindow = getNewWindow("GGB", WINDOW_WIDTH, WINDOW_HEIGHT);
  imageWindow = getSubWindow(mainWindow, 0, 0, WINDOW_WIDTH,
                             WINDOW_HEIGHT);

  console_mode();
  return 0;
}