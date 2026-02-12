#include "command.h"
#include "object.h"

static void perp_eval(const float inputs[4], float *output[3]) {
  const float nx = inputs[0];
  const float ny = inputs[1];
  const float px = inputs[2];
  const float py = inputs[3];
  *output[0] = -ny;
  *output[1] = nx;
  *output[2] = -ny * px + nx * py; // np · (px, py)
}

static void perp_foot_eval(const float inputs[4], float *output[2]) {
  const float nx = inputs[0]; // perp line
  const float ny = inputs[1];
  const float d1 = inputs[2]; // perp line
  const float d2 = inputs[3]; // baseline
  *output[0] = nx * d1 + ny * d2;
  *output[1] = ny * d1 - nx * d2;
}

static void clip_end_point(const float inputs[4], float *t[1]) {
  const float nx = inputs[0];
  const float ny = inputs[1];
  const float px = inputs[2];
  const float py = inputs[3];
  *t[0] = ny * px - nx * py;
}

static void equal(const float input[1], float *output[1]) {
  *output[0] = input[0];
}

void cmd_perp(Command *cmd) {
}