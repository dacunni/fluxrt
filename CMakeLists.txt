cmake_minimum_required(VERSION 3.14)

PROJECT (fluxrt)

option(OPTIMIZE_FOR_NATIVE "Build with -march=native" OFF)

if(OPTIMIZE_FOR_NATIVE)
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
    if(COMPILER_SUPPORTS_MARCH_NATIVE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    endif()
endif()

set(CMAKE_CXX_STANDARD 14 CACHE STRING "C++ version selection")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftime-report")

include_directories(include)
include_directories(src)
include_directories(external/stb)
include_directories(external/tinyobjloader)
include_directories(external/stlloader)
include_directories(external/cpptoml/include)
include_directories(external/generator)

set(SRCS
    src/artifacts.cpp
    src/barycentric.cpp
    src/brdf.cpp
    src/camera.cpp
    src/color.cpp
    src/EnvironmentMap.cpp
    src/GradientEnvironmentMap.cpp
    src/LatLonEnvironmentMap.cpp
    src/CubeMapEnvironmentMap.cpp
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
    src/Ray.cpp
    src/Renderer.cpp
    src/rng.cpp
    src/sensor.cpp
    src/slab.cpp
    src/scene.cpp
    src/scene-toml.cpp
    src/texture.cpp
    src/timer.cpp
    src/Triangle.cpp
    src/TriangleMesh.cpp
    src/TriangleMesh-stl.cpp
    src/TriangleMesh-obj.cpp
    src/TriangleMeshOctree.cpp
    )
ADD_LIBRARY(fluxrt ${SRCS})

add_subdirectory(external/pybind11)
add_subdirectory(bindings/python)

add_subdirectory(app)

add_subdirectory(benchmarks)

enable_testing()
add_subdirectory(unittests)

add_subdirectory(tests)

