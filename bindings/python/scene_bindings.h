void scene_bindings(py::module_ & m)
{
    py::class_<Scene>(m, "Scene")
        // constructors
        .def(py::init<>())
        // methods
        // ...
        // properties
        .def_readwrite("sensor", &Scene::sensor)
        .def_readwrite("camera", &Scene::camera)
        ;

    py::class_<Camera>(m, "Camera")
        ;
        
}

