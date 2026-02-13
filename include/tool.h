#ifndef GGB_COMMAND_H
#define GGB_COMMAND_H

#include "board.h"

typedef struct {
  const char *usage;
  void (*init)(void);
  BoardControl ctrl;
} GeomTool;

GeomId create_point(Vec2 pos, GeomId xy[2]);
GeomId select_point(Vec2 pos, GeomId xy[2]);

void tool_point(GeomTool *);
void tool_line(GeomTool *);
void tool_circle(GeomTool *);
void tool_midpoint(GeomTool *);
void tool_perp(GeomTool *);
void tool_parallel(GeomTool *);
void tool_bisector(GeomTool *);
void tool_circum(GeomTool *);
void tool_isect(GeomTool *);
void tool_delete(GeomTool *);

#endif // GGB_COMMAND_H
