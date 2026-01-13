#ifndef GGB_MATH_H
#define GGB_MATH_H

#include "raymath.h"

typedef Vector2 Vec2;
typedef Vector3 Vec3;

// clang-format off
RMAPI float vec2_length(Vec2 v) { return Vector2Length(v); }
RMAPI float vec2_distance(Vec2 v1, Vector2 v2) { return Vector2Distance(v1, v2); }

// clang-format on
#endif //GGB_MATH_H