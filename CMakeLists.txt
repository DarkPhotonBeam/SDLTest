cmake_minimum_required(VERSION 3.29)
project(sdltest)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 REQUIRED)
find_package(Eigen3 REQUIRED)

include_directories(sdltest ${EIGEN3_INCLUDE_DIRS})
include_directories(sdltest ${SDL2_INCLUDE_DIRS})

add_executable(sdltest main.cpp
        GameInput.cpp
        GameInput.h)

target_link_libraries(sdltest SDL2)
#target_link_libraries(sdltest Eigen3)