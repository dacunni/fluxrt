cmake_minimum_required(VERSION 2.6)

PROJECT (micromath)

include_directories(include)
include_directories(src)
include_directories(external/stb)
include_directories(external/tinyobjloader)
include_directories(external/cpptoml/include)

set(CMAKE_CXX_FLAGS "-std=c++14 -O2")

set(SRCS
    src/artifacts.cpp
    src/barycentric.cpp
    src/brdf.cpp
    src/camera.cpp
    src/color.cpp
    src/filesystem.cpp
    src/fresnel.cpp
    src/image.cpp
    src/integrate.cpp
    src/jacobian.cpp
    src/vec3.cpp
    src/vec4.cpp
    src/material.cpp
    src/optics.cpp
    src/radiometry.cpp
    src/ray.cpp
    src/rng.cpp
    src/sensor.cpp
    src/slab.cpp
    src/scene.cpp
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

enable_testing()
add_subdirectory(unittests)

add_subdirectory(tests)

