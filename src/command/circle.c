#include "command.h"
#include "object.h"
#include <math.h>

static void radius_from_point(const float xyxy[4], float *radius[1]) {
  const float dx = xyxy[2] - xyxy[0];
  const float dy = xyxy[3] - xyxy[1];
  *radius[0] = sqrtf(dx * dx + dy * dy);
}

void cmd_circle(Command *cmd) {
}