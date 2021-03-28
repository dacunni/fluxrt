#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

namespace py = pybind11;

#include "vec3_bindings.h"
#include "ray_bindings.h"
#include "renderer_bindings.h"
#include "sensor_bindings.h"

PYBIND11_MODULE(pyfluxrt, m) {
    m.doc() = "Python bindings for fluxrt";

    vec3_bindings(m);
    ray_bindings(m);
    renderer_bindings(m);
    sensor_bindings(m);
}

