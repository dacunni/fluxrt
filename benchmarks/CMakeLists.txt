cmake_minimum_required(VERSION 2.6)

PROJECT (micromath_benchmark)

option(BUILD_BENCHMARKS "Build benchmarks" ON)

SET(CMAKE_CXX_FLAGS "-std=c++14 -O2")

include_directories(/usr/local/include)
include_directories(..)

link_directories(/usr/local/lib)

set(LIBS benchmark micromath pthread)

add_executable(vector_bench vector.cpp)
target_link_libraries(vector_bench ${LIBS})
add_executable(matrix_bench matrix.cpp)
target_link_libraries(matrix_bench ${LIBS})
add_executable(quaternion_bench quaternion.cpp)
target_link_libraries(quaternion_bench ${LIBS})
add_executable(slab_bench slab.cpp)
target_link_libraries(slab_bench ${LIBS})

add_executable(raysphere_bench raysphere.cpp)
target_link_libraries(raysphere_bench ${LIBS})
add_executable(rayslab_bench rayslab.cpp)
target_link_libraries(rayslab_bench ${LIBS})
add_executable(raytriangle_bench raytriangle.cpp)
target_link_libraries(raytriangle_bench ${LIBS})

add_executable(random_bench random.cpp)
target_link_libraries(random_bench ${LIBS})

add_executable(stdlib_bench stdlib.cpp)
target_link_libraries(stdlib_bench ${LIBS})



