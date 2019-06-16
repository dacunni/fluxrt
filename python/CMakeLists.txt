cmake_minimum_required(VERSION 2.6)

project(micromath_python_bindings)

set(CMAKE_CXX_FLAGS "-std=c++14 -O2")

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})
include_directories(..)

find_package(PythonLibs)
include_directories(${PYTHON_INCLUDE_PATH})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_SWIG_FLAGS "")

set_source_files_properties(micromath.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties(micromath.i PROPERTIES SWIG_FLAGS "-builtin")
#set_property(TARGET ${SWIG_MODULE_micromath_python_bindings_REAL_NAME} PROPERTY CXX_STANDARD_REQUIRED 14)
#swig_add_module(micromath python micromath.i example.cxx)
swig_add_module(micromath python micromath.i)
swig_link_libraries(micromath ${PYTHON_LIBRARIES} micromath)

