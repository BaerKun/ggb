add_subdirectory(raylib)
add_subdirectory(cgraph)

set(3rdparty_DIRS 3rdparty/raylib/src 3rdparty/cgraph/include PARENT_SCOPE)
set(3rdparty_LIBS raylib cgraph_core PARENT_SCOPE)