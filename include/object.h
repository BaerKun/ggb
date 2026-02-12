#ifndef GGB_OBJECT_H
#define GGB_OBJECT_H

#include "graph.h"

typedef struct {
  char name[8];
  ObjectType type;
  bool visible;
  Color color;
  GeomId args[5];
} GeomObject;

void object_module_init();
void object_module_cleanup();

GeomObject* object_get(GeomId id);
GeomId object_create(ObjectType type, const GeomId *args);
void object_delete(GeomId id);
void object_delete_all();
void object_traverse(void (*callback)(GeomId id, const GeomObject *));

#endif // GGB_OBJECT_H