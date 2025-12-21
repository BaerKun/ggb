cmake_minimum_required(VERSION 3.12)
project(GGB LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)

add_subdirectory(3rdparty)

file(GLOB SOURCES "src/*.c" "src/command/*.c")
add_library(core STATIC ${SOURCES})
target_include_directories(core PUBLIC include ${3rdparty_DIRS})

add_executable(ggb main.c)
target_link_libraries(ggb PRIVATE core ${3rdparty_LIBS})
if(UNIX)
    # math.h
    target_link_libraries(ggb PRIVATE m)
endif ()