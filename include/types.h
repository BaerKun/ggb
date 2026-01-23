#ifndef GGB_TYPES_H
#define GGB_TYPES_H

#include <stddef.h>
#include <stdint.h>

// Vector2, 2 components
typedef struct Vector2 {
  float x; // Vector x component
  float y; // Vector y component
} Vector2;

// Vector3, 3 components
typedef struct Vector3 {
  float x; // Vector x component
  float y; // Vector y component
  float z; // Vector z component
} Vector3;

// Vector4, 4 components
typedef struct Vector4 {
  float x; // Vector x component
  float y; // Vector y component
  float z; // Vector z component
  float w; // Vector w component
} Vector4;

// Quaternion, 4 components (Vector4 alias)
typedef Vector4 Quaternion;

// Matrix, 4x4 components, column major, OpenGL style, right-handed
typedef struct Matrix {
  float m0, m4, m8, m12;  // Matrix first row (4 components)
  float m1, m5, m9, m13;  // Matrix second row (4 components)
  float m2, m6, m10, m14; // Matrix third row (4 components)
  float m3, m7, m11, m15; // Matrix fourth row (4 components)
} Matrix;

// Color, 4 components, R8G8B8A8 (32bit)
typedef struct Color {
  unsigned char r; // Color red value
  unsigned char g; // Color green value
  unsigned char b; // Color blue value
  unsigned char a; // Color alpha value
} Color;

// Rectangle, 4 components
typedef struct Rectangle {
  float x;      // Rectangle top-left corner position x
  float y;      // Rectangle top-left corner position y
  float width;  // Rectangle width
  float height; // Rectangle height
} Rectangle;

typedef Vector2 Vec2;
typedef Vector3 Vec3;
typedef int64_t GeomInt;
typedef int64_t GeomId;
typedef size_t GeomSize;

// clang-format on
#endif //GGB_TYPES_H