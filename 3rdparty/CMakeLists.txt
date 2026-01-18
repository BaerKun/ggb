add_library(argparse STATIC argparse/argparse.c)
add_subdirectory(raylib)
add_subdirectory(cgraph)

set(3rdparty_DIRS 3rdparty/argparse 3rdparty/raylib/src 3rdparty/cgraph/include PARENT_SCOPE)
set(3rdparty_LIBS argparse raylib cgraph_core PARENT_SCOPE)