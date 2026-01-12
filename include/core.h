#ifndef GGB_CORE_H
#define GGB_CORE_H

static inline void core_init() {
  void commandline_init();
  void object_module_init();
  commandline_init();
  object_module_init();
}

static inline void core_release() {
  void commandline_cleanup();
  void object_module_cleanup();
  commandline_cleanup();
  object_module_cleanup();
}

#endif //GGB_CORE_H