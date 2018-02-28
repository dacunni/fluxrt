cmake_minimum_required(VERSION 2.6)

PROJECT (micromath_benchmark)

SET(CMAKE_CXX_FLAGS "-std=c++11 -O2")

include_directories(/usr/local/include)
include_directories(..)

link_directories(/usr/local/lib)

set(LIBS
    micromath
    benchmark
   )
#    m
#    c)

add_executable(vector_bench vector.cpp)
target_link_libraries(vector_bench ${LIBS})
add_executable(stdlib_bench stdlib.cpp)
target_link_libraries(stdlib_bench ${LIBS})



