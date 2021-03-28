
void radiometry_bindings(py::module_ & m)
{
    py::class_<RadianceRGB, std::shared_ptr<RadianceRGB>>(m, "RadianceRGB")
        // constructors
        .def(py::init<>())
        .def(py::init<float, float, float>())
        ;
}

