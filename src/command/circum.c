#include "command.h"
#include "object.h"
#include <math.h>

static void circumcircle_eval(const float inputs[6], float *outputs[3]) {
  const float x1 = inputs[0], y1 = inputs[1];
  const float x2 = inputs[2], y2 = inputs[3];
  const float x3 = inputs[4], y3 = inputs[5];
  const float D = 2 * (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2));
  if (fabsf(D) < EPS) return;

  const float sq_xy1 = x1 * x1 + y1 * y1;
  const float sq_xy2 = x2 * x2 + y2 * y2;
  const float sq_xy3 = x3 * x3 + y3 * y3;
  const float cx =
      (sq_xy1 * (y2 - y3) + sq_xy2 * (y3 - y1) + sq_xy3 * (y1 - y2)) / D;
  const float cy =
      (sq_xy1 * (x3 - x2) + sq_xy2 * (x1 - x3) + sq_xy3 * (x2 - x1)) / D;
  const float rx = x1 - cx;
  const float ry = y1 - cy;
  *outputs[0] = cx;
  *outputs[1] = cy;
  *outputs[2] = sqrtf(rx * rx + ry * ry);
}

void cmd_circum(Command *cmd) {
}