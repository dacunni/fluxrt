cmake_minimum_required(VERSION 2.6)

PROJECT (micromath)

include_directories(external/stb)
include_directories(external/tinyobjloader)

SET(CMAKE_CXX_FLAGS "-std=c++11 -O2")

set(SRCS
    barycentric.cpp
    color.cpp
    fresnel.cpp
    image.cpp
    vec3.cpp
    vec4.cpp
    optics.cpp
    rng.cpp
    slab.cpp
    triangle.cpp
    trianglemesh.cpp
    )
ADD_LIBRARY(micromath ${SRCS})

add_subdirectory(benchmarks)

option(BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
if(BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif(BUILD_PYTHON_BINDINGS)

enable_testing()
add_subdirectory(unittests)

add_subdirectory(tests)

