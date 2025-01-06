
INCLUDE_DIRECTORIES(..)

ADD_EXECUTABLE (trace_scene trace_scene.cpp)
#TARGET_LINK_LIBRARIES (trace_scene fluxrt)
TARGET_LINK_LIBRARIES (trace_scene fluxrt raylib)

install(PROGRAMS trace_all DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

