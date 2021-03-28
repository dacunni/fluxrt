
void rng_bindings(py::module_ & m)
{
    py::class_<RNG, std::shared_ptr<RNG>>(m, "RNG")
        // constructors
        .def(py::init<>())
        // methods
        .def("uniform01", &RNG::uniform01)
        .def("uniformRange", &RNG::uniformRange)
        .def("gaussian", 
             [](RNG & rng, float mean, float stddev) {
                return rng.gaussian(mean, stddev);
             })
        .def("gaussian", 
             [](RNG & rng, float stddev) {
                return rng.gaussian(stddev);
             })
        // TODO
        ;
}

