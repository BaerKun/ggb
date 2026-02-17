#include "object.h"
#include "tool.h"
#include "math.h"

static struct {
  ObjectType first_t;
  GeomId first_id;
  GeomId inputs[6];
} internal = {UNKNOWN, -1};

static bool tangent(const float *inputs, const float r1, const float r2, float *outputs[6]) {
  const float x1 = inputs[0];
  const float y1 = inputs[1];
  const float x2 = inputs[3];
  const float y2 = inputs[4];
  const float dx = x2 - x1;
  const float dy = y2 - y1;
  const float sr = r1 + r2;
  const float d2 = dx * dx + dy * dy;
  const float delta = d2 - sr * sr;
  if (delta < 0) return false;
  const float h = sqrtf(delta);
  const float nx1 = (dx * sr + dy * h) / d2;
  const float nx2 = (dx * sr - dy * h) / d2;
  const float ny1 = (dy * sr - dx * h) / d2;
  const float ny2 = (dy * sr + dx * h) / d2;
  *outputs[0] = nx1;
  *outputs[1] = ny1;
  *outputs[2] = nx1 * x2 + ny1 * y2 - r2;
  *outputs[3] = nx2;
  *outputs[4] = ny2;
  *outputs[5] = nx2 * x2 + ny2 * y2 - r2;
  return true;
}

static bool tangent_circle_point(const float inputs[5], float *outputs[6]) {
  const float r = inputs[2];
  return tangent(inputs, r, 0, outputs);
}

static bool tangent_circles_inner(const float inputs[6], float *outputs[6]) {
  const float r1 = inputs[2];
  const float r2 = inputs[5];
  return tangent(inputs, r1, -r2, outputs);
}

static bool tangent_circles_outer(const float inputs[6], float *outputs[6]) {
  const float r1 = inputs[2];
  const float r2 = inputs[5];
  return tangent(inputs, r1, r2, outputs);
}

static void create_tangents_cp() {
  GeomId args[10];
  init_line(args);
  init_line(args + 5);

  const GeomId outputs[6] = {args[0], args[1], args[2],
                             args[5], args[6], args[7]};

  graph_add_constraint(5, internal.inputs, 6, outputs, tangent_circle_point);
  board_add_object(object_create(LINE, args));
  board_add_object(object_create(LINE, args + 5));
}

static void create_tangents_cc() {
  GeomId args[20];
  init_line(args);
  init_line(args + 5);
  init_line(args + 10);
  init_line(args + 15);

  const GeomId outputs[12] = {args[0],  args[1],  args[2],  args[5],
                              args[6],  args[7],  args[10], args[11],
                              args[12], args[15], args[16], args[17]};

  graph_add_constraint(6, internal.inputs, 6, outputs, tangent_circles_inner);
  graph_add_constraint(6, internal.inputs, 6, outputs + 6,
                       tangent_circles_outer);
  board_add_object(object_create(LINE, args));
  board_add_object(object_create(LINE, args + 5));
  board_add_object(object_create(LINE, args + 10));
  board_add_object(object_create(LINE, args + 15));
}

static void tangent_reset() {
  if (internal.first_id != -1) {
    board_deselect_object(internal.first_id);
    internal.first_t = UNKNOWN;
    internal.first_id = -1;
  }
}

void tangent_ctrl(const Vec2 pos, const MouseEvent event) {
  if (event != MOUSE_PRESS) return;

  if (internal.first_id == -1) {
    GeomId id = board_find_object(POINT | CIRCLE, pos);
    if (id == -1) {
      id = create_point(pos, internal.inputs + 3);
      internal.first_t = POINT;
    } else {
      const GeomObject *obj = object_get(id);
      if (obj->type == POINT) {
        internal.first_t = POINT;
        copy_args(internal.inputs + 3, obj->args, 2);
      } else {
        internal.first_t = CIRCLE;
        copy_args(internal.inputs, obj->args, 3);
      }
    }
    internal.first_id = id;
    board_select_object(id);
    return;
  }

  if (internal.first_t == POINT) {
    const GeomId id = board_find_object(CIRCLE, pos);
    if (id == -1) return;
    const GeomObject *obj = object_get(id);
    copy_args(internal.inputs, obj->args, 3);
    create_tangents_cp();
  } else {
    const GeomId id = board_find_object(POINT | CIRCLE, pos);
    if (id == -1) {
      create_point(pos, internal.inputs + 3);
      create_tangents_cp();
    } else {
      const GeomObject *obj = object_get(id);
      if (obj->type == POINT) {
        copy_args(internal.inputs + 3, obj->args, 2);
        create_tangents_cp();
      } else {
        copy_args(internal.inputs + 3, obj->args, 3);
        create_tangents_cc();
      }
    }
  }
  tangent_reset();
}

void tool_tangent(GeomTool *tool) {
  tool->usage = "tangents: select point or circle";
  tool->ctrl = tangent_ctrl;
  tool->reset = tangent_reset;
}