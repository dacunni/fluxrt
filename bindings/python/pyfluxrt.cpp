#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

namespace py = pybind11;

// stdlib
#include <sstream>

// fluxrt
class PYBIND11_EXPORT Camera;
#include "vec3.h"
#include "scene.h"
#include "Ray.h"
#include "Renderer.h"
#include "rng.h"
#include "scene.h"
#include "sensor.h"

// Bindings
#include "vec3_bindings.h"
#include "ray_bindings.h"
#include "renderer_bindings.h"
#include "sensor_bindings.h"
#include "scene_bindings.h"

PYBIND11_MODULE(pyfluxrt, m) {
    m.doc() = "Python bindings for fluxrt";

    vec3_bindings(m);
    ray_bindings(m);
    renderer_bindings(m);
    sensor_bindings(m);
    scene_bindings(m);
}

