#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include "vec3.h"

namespace py = pybind11;

PYBIND11_MODULE(pyfluxrt, m) {
    m.doc() = "Python bindings for fluxrt";

    py::class_<vec3>(m, "vec3")
        // constructors
        .def(py::init<>())
        .def(py::init<const vec3 &>())
        .def(py::init<float, float, float>())
        // factories
        .def_static("zero", &vec3::zero)
        // properties
        .def_readwrite("x", &vec3::x)
        .def_readwrite("y", &vec3::y)
        .def_readwrite("z", &vec3::z)
        // methods
        .def("set", &vec3::set)
        .def("negated", &vec3::negated)
        .def("negate", &vec3::negate)
        .def("magnitude_sq", &vec3::magnitude_sq)
        .def("magnitude", &vec3::magnitude)
        .def("normalized", &vec3::normalized)
        .def("normalize", &vec3::normalize)
        .def("invertedComponents", &vec3::invertedComponents)
        .def("minElement", &vec3::minElement)
        .def("maxElement", &vec3::maxElement)
        .def("string", &vec3::string)
        .def("isZeros", &vec3::isZeros)
        .def("hasNaN", &vec3::hasNaN)
        .def("hasInf", &vec3::hasInf)

        // operators
        .def(py::self == py::self)
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self * float())
        .def(float() * py::self)
        .def(py::self / float())

        // repr
        .def("__repr__", [](const vec3 & v) {
                 return v.string();
             });
        ;

    // vec3 free functions
    m.def("dot", &dot);
    m.def("absDot", &absDot);
    m.def("clampedDot", &clampedDot);
    m.def("cross", &cross);
    m.def("blend", &blend);
    m.def("mirror", &mirror);
    m.def("refract", &refract);
    m.def("interp", &interp);

}

