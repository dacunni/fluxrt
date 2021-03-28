
void material_bindings(py::module_ & m)
{
    py::class_<Medium, std::shared_ptr<Medium>>(m, "Medium")
        // constructors
        .def(py::init<>())
        // methods
        .def_readwrite("indexOfRefraction", &Medium::indexOfRefraction)
        // TODO
        ;

    m.attr("VaccuumMedium") = py::cast(VaccuumMedium);
}

