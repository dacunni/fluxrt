
void vec2_bindings(py::module_ & m)
{
    py::class_<vec2, std::shared_ptr<vec2>>(m, "vec2")
        // constructors
        .def(py::init<>())
        .def(py::init<const vec2 &>())
        .def(py::init<float, float>())
        // factories
        .def_static("zero", &vec2::zero)
        // properties
        .def_readwrite("x", &vec2::x)
        .def_readwrite("y", &vec2::y)
        // methods
        .def("set", &vec2::set)
        .def("negated", &vec2::negated)
        .def("negate", &vec2::negate)
        .def("magnitude_sq", &vec2::magnitude_sq)
        .def("magnitude", &vec2::magnitude)
        .def("normalized", &vec2::normalized)
        .def("normalize", &vec2::normalize)
        .def("minElement", &vec2::minElement)
        .def("maxElement", &vec2::maxElement)
        .def("string", &vec2::string)

        // operators
        .def(py::self == py::self)
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self * float())
        .def(float() * py::self)
        .def(py::self / float())

        // repr
        .def("__repr__", [](const vec2 & v) {
                 return v.string();
             });
        ;

    // vec2 free functions
    m.def("dot", [](const vec2 & a, const vec2 & b) { return dot(a, b); });
    m.def("clampedDot", [](const vec2 & a, const vec2 & b) { return clampedDot(a, b); });
    m.def("blend", [](const vec2 & a, float s, const vec2 & b, float t) { return blend(a, s, b, t); });
    m.def("interp", [](const vec2 & a, const vec2 & b, float alpha) { return interp(a, b, alpha); });
}

