
void radiometry_bindings(py::module_ & m)
{
    py::class_<RadianceRGB, std::shared_ptr<RadianceRGB>>(m, "RadianceRGB")
        // constructors
        .def(py::init<>())
        .def(py::init<float, float, float>())
        .def_property("r", [](RadianceRGB & v) { return v.r; },
                           [](RadianceRGB & v, float r) { v.r = r; })
        .def_property("g", [](RadianceRGB & v) { return v.g; },
                           [](RadianceRGB & v, float g) { v.g = g; })
        .def_property("b", [](RadianceRGB & v) { return v.b; },
                           [](RadianceRGB & v, float b) { v.b = b; })
        ;
}

