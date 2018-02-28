cmake_minimum_required(VERSION 2.6)

PROJECT (micromath_tests)

SET(CMAKE_CXX_FLAGS "-std=c++11 -O2")

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
include_directories(..)

add_executable(vec3 vec3.cpp)
add_executable(vec4 vec4.cpp)
add_executable(quaternion quaternion.cpp)

#link_directories(/usr/local/lib)

set(LIBS
    micromath
    ${GTEST_BOTH_LIBRARIES}
   )
#    m
#    c)

target_link_libraries(vec3 ${LIBS})
target_link_libraries(vec4 ${LIBS})
target_link_libraries(quaternion ${LIBS})

add_test(AllTestsInVec3 vec3)
add_test(AllTestsInVec4 vec4)
add_test(AllTestsInQuaternion quaternion)


