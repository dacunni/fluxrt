
void rng_bindings(py::module_ & m)
{
    py::class_<RNG, std::shared_ptr<RNG>>(m, "RNG")
        // constructors
        .def(py::init<>())
        // methods
        //   1D
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
        //   2D
        .def("uniformUnitCircle",
            [](RNG & rng) {
                return rng.uniformUnitCircle();
            })
        .def("uniformCircle",
            [](RNG & rng, float radius) {
                return rng.uniformCircle(radius);
            })
        .def("uniformRectangle",
            [](RNG & rng, float xmin, float xmax, float ymin, float ymax) {
                return rng.uniformRectangle(xmin, xmax, ymin, ymax);
            })
        .def("gaussian2D", 
             [](RNG & rng, float mean, float stddev) {
                return rng.gaussian2D(mean, stddev);
             })
        .def("gaussian2D", 
             [](RNG & rng, float stddev) {
                return rng.gaussian2D(stddev);
             })
        .def("uniformSurfaceUnitSphere",
             [](RNG & rng) {
                return rng.uniformSurfaceUnitSphere();
             })
        .def("uniformSurfaceUnitHalfSphere",
             [](RNG & rng, Direction3 & n) {
                return rng.uniformSurfaceUnitHalfSphere(n);
             })
        .def("cosineAboutDirection",
             [](RNG & rng, Direction3 & n) {
                return rng.cosineAboutDirection(n);
             })
        .def("gaussian3D",
             [](RNG & rng, float mean, float stddev) {
                return rng.gaussian3D(mean, stddev);
             })
        .def("gaussian3D",
             [](RNG & rng, float stddev) {
                return rng.gaussian3D(stddev);
             })
        ;
}

