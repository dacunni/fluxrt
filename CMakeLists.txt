cmake_minimum_required(VERSION 2.6)

PROJECT (micromath)

option(OPTIMIZE_FOR_NATIVE "Build with -march=native" OFF)

if(OPTIMIZE_FOR_NATIVE)
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
    if(COMPILER_SUPPORTS_MARCH_NATIVE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    endif()
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftime-report")

include_directories(include)
include_directories(src)
include_directories(external/stb)
include_directories(external/tinyobjloader)
include_directories(external/stlloader)
include_directories(external/cpptoml/include)

set(SRCS
    src/artifacts.cpp
    src/barycentric.cpp
    src/brdf.cpp
    src/camera.cpp
    src/color.cpp
    src/environmentmap.cpp
    src/filesystem.cpp
    src/fresnel.cpp
    src/image.cpp
    src/integrate.cpp
    src/jacobian.cpp
    src/vec2.cpp
    src/vec3.cpp
    src/vec4.cpp
    src/material.cpp
    src/matrix.cpp
    src/optics.cpp
    src/radiometry.cpp
    src/ray.cpp
    src/rng.cpp
    src/sensor.cpp
    src/slab.cpp
    src/scene.cpp
    src/scene-toml.cpp
    src/texture.cpp
    src/timer.cpp
    src/triangle.cpp
    src/trianglemesh.cpp
    src/trianglemesh-stl.cpp
    src/trianglemesh-obj.cpp
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

