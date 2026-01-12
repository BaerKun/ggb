cmake_minimum_required(VERSION 3.12)
project(GGB LANGUAGES C)

set(CMAKE_C_STANDARD 11)

add_subdirectory(3rdparty)

file(GLOB_RECURSE SOURCES "src/*.c")
add_executable(ggb ${SOURCES})
target_include_directories(ggb PUBLIC include ${3rdparty_DIRS})
target_link_libraries(ggb PRIVATE ${3rdparty_LIBS})
if(UNIX)
    # math.h
    target_link_libraries(core PRIVATE m)
endif ()