void scene_bindings(py::module_ & m)
{
    py::class_<Scene, std::shared_ptr<Scene>>(m, "Scene")
        // constructors
        .def(py::init<>())
        // methods
        .def("print", &Scene::print,
             py::call_guard<py::scoped_ostream_redirect>())
        // ...
        // properties
        .def_readwrite("sensor", &Scene::sensor)
        .def_readwrite("camera", &Scene::camera)
        ;

    m.def("loadSceneFromFile", 
          [](Scene & scene, const std::string & filename) {
              if(!loadSceneFromFile(scene, filename)) {
                  throw std::runtime_error("Unable to load scene from file" + filename);
              }
          },
          py::call_guard<py::scoped_ostream_redirect>())
        ;
}

