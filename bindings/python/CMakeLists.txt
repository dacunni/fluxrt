PROJECT (fluxrt_python_bindings)

include_directories(../../include)
pybind11_add_module(pyfluxrt pyfluxrt.cpp)

target_link_libraries(pyfluxrt PRIVATE fluxrt)
