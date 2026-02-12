#include "command.h"
#include "object.h"

static void parallel_eval(const float inputs[4], float *outputs[3]) {
  const float nx = inputs[0];
  const float ny = inputs[1];
  const float px = inputs[2];
  const float py = inputs[3];
  *outputs[0] = nx;
  *outputs[1] = ny;
  *outputs[2] = nx * px + ny * py; // parallel line dd
}

void cmd_parallel(Command *cmd) {
}