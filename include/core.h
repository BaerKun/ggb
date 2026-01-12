#ifndef GGB_CORE_H
#define GGB_CORE_H

#include "commandline.h"
#include "object.h"

static inline void core_init() {
  commandline_init();
  object_module_init();
}

static inline void core_release() {
  commandline_cleanup();
  object_module_cleanup();
}

#endif //GGB_CORE_H