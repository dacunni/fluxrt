cmake_minimum_required(VERSION 2.6)

project(micromath_chaiscript_bindings)

set(CMAKE_CXX_FLAGS "-std=c++14 -O2")

add_executable(chairun chairun.cpp)
target_link_libraries(chairun micromath)

