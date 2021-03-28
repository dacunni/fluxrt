
void sensor_bindings(py::module_ & m)
{
    py::class_<Sensor, std::shared_ptr<Sensor>>(m, "Sensor")
        // constructors
        .def(py::init<>())
        .def(py::init<uint32_t, uint32_t>())
        // methods
        .def("forEachPixel", &Sensor::forEachPixel)
        .def("forEachPixelInRect", &Sensor::forEachPixelInRect)
        .def("forEachPixelTiled", &Sensor::forEachPixelTiled)
        .def("forEachPixelThreaded", &Sensor::forEachPixelThreaded)
        .def("forEachPixelTiledThreaded", &Sensor::forEachPixelTiledThreaded)
        .def("pixelStandardImageLocation",
             static_cast<vec2(Sensor::*)(float, float)>(&Sensor::pixelStandardImageLocation))
        .def("pixelStandardImageLocation",
             static_cast<vec2(Sensor::*)(const vec2 &)>(&Sensor::pixelStandardImageLocation))
        .def("aspectRatio", &Sensor::aspectRatio)
        .def("print", &Sensor::print,
             py::call_guard<py::scoped_ostream_redirect>())
        // properties
        .def_readwrite("pixelwidth", &Sensor::pixelwidth)
        .def_readwrite("pixelheight", &Sensor::pixelheight)
        ;
}

