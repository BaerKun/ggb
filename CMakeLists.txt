cmake_minimum_required(VERSION 3.12)
project(GGB)

add_subdirectory(3rdparty)

find_package(OpenCV REQUIRED)

add_library(gl STATIC src/graphical.cpp)
target_link_directories(gl PUBLIC ${OpenCV_DIRS})
target_include_directories(gl PRIVATE include)
target_link_libraries(gl PRIVATE ${OpenCV_LIBS})

file(GLOB SOURCES "src/*.c")

add_library(core STATIC ${SOURCES})
target_include_directories(core PUBLIC include ${3rdparty_DIRS})

add_executable(ggb main.c)
target_link_libraries(ggb PRIVATE core gl ${3rdparty_LIBS})
if(UNIX)
    # math.h
    target_link_libraries(ggb PRIVATE m)
endif ()