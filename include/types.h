#ifndef GGB_MATH_H
#define GGB_MATH_H

#include "raymath.h"
#include <stdint.h>

typedef Vector2 Vec2;
typedef Vector3 Vec3;
typedef int64_t GeomInt;
typedef int64_t GeomId;
typedef uint64_t GeomSize;

#define INLINE static inline

// clang-format off
INLINE Vec2 vec2_add(Vec2 v1, Vec2 v2) { return Vector2Add(v1, v2); }
INLINE Vec2 vec2_sub(Vec2 v1, Vec2 v2) { return Vector2Subtract(v1, v2); }
INLINE float vec2_length(Vec2 v) { return Vector2Length(v); }
INLINE float vec2_distance(Vec2 v1, Vector2 v2) { return Vector2Distance(v1, v2); }

// clang-format on
#endif //GGB_MATH_H