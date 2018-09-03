cmake_minimum_required(VERSION 2.6)

PROJECT (micromath)

SET(CMAKE_CXX_FLAGS "-std=c++11 -O2")

set(SRCS
    vec3.cpp
    vec4.cpp
    slab.cpp
    triangle.cpp
    )
ADD_LIBRARY(micromath ${SRCS})

add_subdirectory(benchmarks)
add_subdirectory(python)

enable_testing()
add_subdirectory(unittests)

