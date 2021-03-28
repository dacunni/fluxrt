void camera_bindings(py::module_ & m)
{
    py::class_<Camera, std::shared_ptr<Camera>>(m, "Camera")
        // methods
        .def("rayThroughStandardImagePlane",
             [](Camera & camera, const vec2 & v, const vec2 & blur) {
                return camera.rayThroughStandardImagePlane(v, blur);
             })
        .def_readwrite("position", &Camera::position)
        .def_readwrite("direction", &Camera::direction)
        .def_readwrite("up", &Camera::up)
        .def_readwrite("right", &Camera::right)
        ;

    py::class_<PinholeCamera, Camera, std::shared_ptr<PinholeCamera>>(m, "PinholeCamera")
        // constructors
        .def(py::init<>())
        .def(py::init<float, float>())
        // methods
        // ...
        // properties
        .def_readwrite("hfov", &PinholeCamera::hfov)
        .def_readwrite("vfov", &PinholeCamera::vfov)
        .def_readwrite("applyLensBlur", &PinholeCamera::applyLensBlur)
        .def_readwrite("focusDistance", &PinholeCamera::focusDistance)
        .def_readwrite("focusDivergence", &PinholeCamera::focusDivergence)
        ;

    py::class_<OrthoCamera, Camera, std::shared_ptr<OrthoCamera>>(m, "OrthoCamera")
        // constructors
        .def(py::init<>())
        .def(py::init<float, float>())
        // methods
        // ...
        // properties
        .def_readwrite("hsize", &OrthoCamera::hsize)
        .def_readwrite("vsize", &OrthoCamera::vsize)
        ;
}

