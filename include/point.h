#ifndef GGB_POINT_H
#define GGB_POINT_H

#include "types.h"

typedef struct PointObject_ PointObject;
struct PointObject_ {
  Vec2 coord;
  GeomInt n_dep;
  Vec2 (*eval)(GeomInt, const Vec2 *);
  GeomId deps[6];
};

typedef struct {
  GeomInt n_pts;
  GeomId *pts;
  Vec2 (*eval)(GeomInt, const Vec2 *);
} Constraint;

void point_module_init(GeomSize init_size);
void point_module_cleanup();
GeomId point_create(Vec2 coord, Constraint cons);
Vec2 point_get_coord(GeomId id);
void point_delete(GeomId id);

#endif // GGB_POINT_H