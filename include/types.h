
#ifndef GGB_TYPES_H
#define GGB_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef int32_t GeomInt;
typedef int32_t GeomId;
typedef uint32_t GeomSize;

typedef struct Vector2 {
  float x;
  float y;
} Vector2, Vec2;

typedef struct Color {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
} Color;

typedef enum {
  UNKNOWN = 0,
  POINT = 1,
  CIRCLE = 2,
  LINE = 4,
  ANY = 7
} ObjectType;

#endif //GGB_TYPES_H