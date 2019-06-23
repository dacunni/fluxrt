cmake_minimum_required(VERSION 2.6)

PROJECT (micromath)

include_directories(include)
include_directories(src)
include_directories(external/stb)
include_directories(external/tinyobjloader)

set(CMAKE_CXX_FLAGS "-std=c++14 -O2")

set(SRCS
    src/barycentric.cpp
    src/color.cpp
    src/fresnel.cpp
    src/image.cpp
    src/integrate.cpp
    src/vec3.cpp
    src/vec4.cpp
    src/material.cpp
    src/optics.cpp
    src/radiometry.cpp
    src/rng.cpp
    src/slab.cpp
    src/triangle.cpp
    src/trianglemesh.cpp
    src/trianglemeshoctree.cpp
    )
ADD_LIBRARY(micromath ${SRCS})

add_subdirectory(benchmarks)

option(BUILD_PYTHON_BINDINGS "Build Python bindings" OFF)
if(BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif(BUILD_PYTHON_BINDINGS)

option(BUILD_CHAISCRIPT_BINDINGS "Build ChaiScript bindings" ON)
if(BUILD_CHAISCRIPT_BINDINGS)
    include_directories(external/chaiscript/include)
    add_subdirectory(chaiscript)
endif(BUILD_CHAISCRIPT_BINDINGS)

enable_testing()
add_subdirectory(unittests)

add_subdirectory(tests)

