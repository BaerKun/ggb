#include "command.h"
#include "object.h"
#include <math.h>

static void line_from_2points(const float xyxy[4], float *line[3]) {
  const float x1 = xyxy[0];
  const float y1 = xyxy[1];
  const float x2 = xyxy[2];
  const float y2 = xyxy[3];
  const float dx = x2 - x1;
  const float dy = y2 - y1;
  const float dist = sqrtf(dx * dx + dy * dy);
  if (dist < EPS) return;

  const float nx = -dy / dist;
  const float ny = dx / dist;
  *line[0] = nx;
  *line[1] = ny;
  *line[2] = nx * x1 + ny * y1; // dd = n · (x, y)
}

static void clip_end_point(const float inputs[4], float *t[1]) {
  const float nx = inputs[0];
  const float ny = inputs[1];
  const float px = inputs[2];
  const float py = inputs[3];
  *t[0] = ny * px - nx * py; // (ny, -nx) · (px, py)
}

static void line_ctrl(const Vec2 pos, const bool click) {
  if (!click) return;
}

void cmd_line(Command *cmd) {
  cmd->usage = "";
}