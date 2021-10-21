
void envmap_bindings(py::module_ & m)
{
    py::class_<EnvironmentMap, std::shared_ptr<EnvironmentMap>>(m, "EnvironmentMap")
        // constructors
        .def(py::init<>())
        // methods
        .def("importanceSample", &EnvironmentMap::importanceSample)
        .def("canImportanceSample", &EnvironmentMap::canImportanceSample)
        ;

    py::class_<LatLonEnvironmentMap, EnvironmentMap, std::shared_ptr<LatLonEnvironmentMap>>(m, "LatLonEnvironmentMap")
        // constructors
        .def(py::init<>())
        // methods
        .def("loadFromFile", &LatLonEnvironmentMap::loadFromFile)
        .def("loadFromImage", &LatLonEnvironmentMap::loadFromImage)
        .def("importanceSample", &LatLonEnvironmentMap::importanceSample)
        .def("canImportanceSample", &LatLonEnvironmentMap::canImportanceSample)
        .def("saveDebugImages", &LatLonEnvironmentMap::saveDebugImages)
        ;
}

