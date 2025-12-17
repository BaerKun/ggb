add_library(argparse STATIC argparse/argparse.c)

set(3rdparty_DIRS 3rdparty/argparse PARENT_SCOPE)
set(3rdparty_LIBS argparse PARENT_SCOPE)