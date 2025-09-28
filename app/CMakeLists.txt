
INCLUDE_DIRECTORIES(..)

ADD_EXECUTABLE (trace_scene trace_scene.cpp)
TARGET_LINK_LIBRARIES (trace_scene fluxrt)



install(PROGRAMS trace_all DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

