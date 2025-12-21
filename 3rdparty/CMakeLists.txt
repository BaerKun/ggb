add_library(argparse STATIC argparse/argparse.c)
add_subdirectory(raylib)

set(3rdparty_DIRS 3rdparty/argparse 3rdparty/raylib/src PARENT_SCOPE)
set(3rdparty_LIBS argparse raylib PARENT_SCOPE)