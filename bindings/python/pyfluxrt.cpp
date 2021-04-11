#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/iostream.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>

namespace py = pybind11;

// stdlib
#include <sstream>

// fluxrt
class PYBIND11_EXPORT Camera;
#include "vec2.h"
#include "vec3.h"
#include "scene.h"
#include "Ray.h"
#include "Renderer.h"
#include "rng.h"
#include "scene.h"
#include "sensor.h"
#include "radiometry.h"
#include "material.h"
#include "fresnel.h"
#include "optics.h"

// Bindings
#include "vec2_bindings.h"
#include "vec3_bindings.h"
#include "ray_bindings.h"
#include "renderer_bindings.h"
#include "sensor_bindings.h"
#include "scene_bindings.h"
#include "camera_bindings.h"
#include "radiometry_bindings.h"
#include "rng_bindings.h"
#include "material_bindings.h"
#include "fresnel_bindings.h"
#include "optics_bindings.h"

PYBIND11_MODULE(pyfluxrt, m) {
    m.doc() = "Python bindings for fluxrt";

    vec2_bindings(m);
    vec3_bindings(m);
    ray_bindings(m);
    renderer_bindings(m);
    sensor_bindings(m);
    scene_bindings(m);
    camera_bindings(m);
    radiometry_bindings(m);
    rng_bindings(m);
    material_bindings(m);
    fresnel_bindings(m);
    optics_bindings(m);
}

