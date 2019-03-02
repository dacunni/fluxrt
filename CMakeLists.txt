cmake_minimum_required(VERSION 2.6)

PROJECT (micromath)

include_directories(external/stb)

SET(CMAKE_CXX_FLAGS "-std=c++11 -O2")

set(SRCS
    color.cpp
    fresnel.cpp
    image.cpp
    vec3.cpp
    vec4.cpp
    optics.cpp
    rng.cpp
    slab.cpp
    triangle.cpp
    )
ADD_LIBRARY(micromath ${SRCS})

add_subdirectory(benchmarks)
add_subdirectory(python)

enable_testing()
add_subdirectory(unittests)

add_subdirectory(tests)

