
void vec3_bindings(py::module_ & m)
{
    py::class_<vec3, std::shared_ptr<vec3>>(m, "vec3")
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
    m.def("dot", [](const vec3 & a, const vec3 & b) { return dot(a, b); });
    m.def("absDot", [](const vec3 & a, const vec3 & b) { return absDot(a, b); });
    m.def("clampedDot", [](const vec3 & a, const vec3 & b) { return clampedDot(a, b); });
    m.def("cross", [](const vec3 & a, const vec3 & b) { return cross(a, b); });
    m.def("blend", [](const vec3 & a, float s, const vec3 & b, float t) { return blend(a, s, b, t); });
    m.def("mirror", [](const vec3 & a, const vec3 & n) { return mirror(a, n); });
    m.def("refract", [](const vec3 & a, const vec3 & n, float n1, float n2) { return refract(a, n, n1, n2); });
    m.def("interp", [](const vec3 & a, const vec3 & b, float alpha) { return interp(a, b, alpha); });

    py::class_<Position3, vec3, std::shared_ptr<Position3>>(m, "Position3")
        // constructors
        .def(py::init<>())
        .def(py::init<const vec3 &>())
        .def(py::init<float, float, float>())
        ;

    py::class_<Direction3, vec3, std::shared_ptr<Direction3>>(m, "Direction3")
        // constructors
        .def(py::init<>())
        .def(py::init<const vec3 &>())
        .def(py::init<float, float, float>())
        .def("normalized", &Direction3::normalized)
        ;
}

