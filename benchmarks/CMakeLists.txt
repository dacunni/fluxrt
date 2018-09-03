cmake_minimum_required(VERSION 2.6)

PROJECT (micromath_benchmark)

SET(CMAKE_CXX_FLAGS "-std=c++11 -O2")

include_directories(/usr/local/include)
include_directories(..)

link_directories(/usr/local/lib)

set(LIBS benchmark micromath)

add_executable(vector_bench vector.cpp)
target_link_libraries(vector_bench ${LIBS})
add_executable(matrix_bench matrix.cpp)
target_link_libraries(matrix_bench ${LIBS})
add_executable(quaternion_bench quaternion.cpp)
target_link_libraries(quaternion_bench ${LIBS})
add_executable(slab_bench slab.cpp)
target_link_libraries(slab_bench ${LIBS})

add_executable(raysphere raysphere.cpp)
target_link_libraries(raysphere ${LIBS})
add_executable(rayslab rayslab.cpp)
target_link_libraries(rayslab ${LIBS})
add_executable(raytriangle raytriangle.cpp)
target_link_libraries(raytriangle ${LIBS})

add_executable(stdlib_bench stdlib.cpp)
target_link_libraries(stdlib_bench ${LIBS})



