#include "object.h"
#include "tool.h"
#include <math.h>

static struct {
  ObjectType first_t;
  GeomId first_id;
  GeomId inputs[6];
} internal = {UNKNOWN, -1};

static void isect_line_line(const float inputs[6], float *outputs[2]) {
  const float nx1 = inputs[0];
  const float ny1 = inputs[1];
  const float dd1 = inputs[2];
  const float nx2 = inputs[3];
  const float ny2 = inputs[4];
  const float dd2 = inputs[5];
  const float D = nx1 * ny2 - nx2 * ny1;
  if (fabsf(D) < EPS) return;
  *outputs[0] = (ny2 * dd1 - ny1 * dd2) / D;
  *outputs[1] = (nx1 * dd2 - nx2 * dd1) / D;
}

static void isect_line_circle(const float inputs[6], float *outputs[4]) {
  const float nx = inputs[0];
  const float ny = inputs[1];
  const float dd = inputs[2];
  const float cx = inputs[3];
  const float cy = inputs[4];
  const float r = inputs[5];
  const float cross = nx * cy - ny * cx;
  const float tmp = nx * cx + ny * cy - dd;
  const float B = 2 * cross;
  const float C = cross * cross + (tmp + r) * (tmp - r);
  const float delta = B * B - 4 * C;
  if (delta < EPS) return;
  const float sqrt_delta = sqrtf(delta);
  const float t1 = (-B + sqrt_delta) / 2.f;
  const float t2 = (-B - sqrt_delta) / 2.f;
  *outputs[0] = nx * dd + ny * t1;
  *outputs[1] = ny * dd - nx * t1;
  *outputs[2] = nx * dd + ny * t2;
  *outputs[3] = ny * dd - nx * t2;
}

static void isect_circle_circle(const float inputs[6], float *outputs[4]) {
  const float x1 = inputs[0];
  const float y1 = inputs[1];
  const float r1 = inputs[2];
  const float x2 = inputs[3];
  const float y2 = inputs[4];
  const float r2 = inputs[5];
  const float dx = x2 - x1;
  const float dy = y2 - y1;
  const float d = sqrtf(dx * dx + dy * dy);
  if (d > r1 + r2) return;
  const float a = (d * d + r1 * r1 - r2 * r2) / (2 * d);
  const float h = sqrtf(r1 * r1 - a * a);
  const float ux = dx / d;
  const float uy = dy / d;
  const float px = x1 + a * ux;
  const float py = y1 + a * uy;
  *outputs[0] = px + h * uy;
  *outputs[1] = py - h * ux;
  *outputs[2] = px - h * uy;
  *outputs[3] = py + h * ux;
}

static void args_copy3(GeomId dst[3], const GeomId src[3]) {
  dst[0] = src[0];
  dst[1] = src[1];
  dst[2] = src[2];
}

static void isect_reset() {
  if (internal.first_id != -1) {
    board_deselect_object(internal.first_id);
    internal.first_t = UNKNOWN;
    internal.first_id = -1;
  }
}

static void isect_ctrl(const Vec2 pos, const MouseEvent event) {
  if (event != MOUSE_PRESS) return;

  const GeomId id = board_find_object(LINE | CIRCLE, pos);
  if (id == -1) return;

  const GeomObject *obj = object_get(id);
  if (internal.first_id == -1) {
    if (obj->type == LINE) {
      args_copy3(internal.inputs, obj->args);
      internal.first_t = LINE;
    } else {
      args_copy3(internal.inputs + 3, obj->args);
      internal.first_t = CIRCLE;
    }
    internal.first_id = id;
    board_select_object(id);
    return;
  }

  GeomId args[4];
  args[0] = graph_add_value(0);
  args[1] = graph_add_value(0);

  if (internal.first_t == LINE) {
    args_copy3(internal.inputs + 3, obj->args);
    if (obj->type == LINE) {
      graph_add_constraint(6, internal.inputs, 2, args, isect_line_line);
      board_add_object(object_create(POINT, args));
    } else {
      args[2] = graph_add_value(0);
      args[3] = graph_add_value(0);
      graph_add_constraint(6, internal.inputs, 4, args, isect_line_circle);
      board_add_object(object_create(POINT, args));
      board_add_object(object_create(POINT, args + 2));
    }
    isect_reset();
    return;
  }

  args[2] = graph_add_value(0);
  args[3] = graph_add_value(0);
  args_copy3(internal.inputs, obj->args);
  if (obj->type == LINE) {
    graph_add_constraint(6, internal.inputs, 4, args, isect_line_circle);
  } else {
    graph_add_constraint(6, internal.inputs, 4, args, isect_circle_circle);
  }
  board_add_object(object_create(POINT, args));
  board_add_object(object_create(POINT, args + 2));
  isect_reset();
}

void tool_isect(GeomTool *tool) {
  tool->usage = "intersection point: select two lines, circles or both";
  tool->ctrl = isect_ctrl;
  tool->reset = isect_reset;
}