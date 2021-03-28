
void ray_bindings(py::module_ & m)
{
    py::class_<Ray, std::shared_ptr<Ray>>(m, "Ray")
        // constructors
        .def(py::init<>())
        .def(py::init<const Position3 &, const Direction3 &>())
        // methods
        .def("pointAt", &Ray::pointAt)
        // properties
        .def_readwrite("origin", &Ray::origin)
        .def_readwrite("direction", &Ray::direction)
        // repr
        .def("__repr__", [](const Ray & r) {
                std::stringstream ss;
                ss << r;
                return ss.str();
            });
        ;

    py::class_<RayIntersection, std::shared_ptr<RayIntersection>>(m, "RayIntersection")
        // constructors
        .def(py::init<>())
        // properties
        .def_readwrite("ray", &RayIntersection::ray)
        .def_readwrite("position", &RayIntersection::position)
        .def_readwrite("normal", &RayIntersection::normal)
        .def_readwrite("tangent", &RayIntersection::tangent)
        .def_readwrite("bitangent", &RayIntersection::bitangent)
        .def_readwrite("distance", &RayIntersection::distance)
        .def_readwrite("material", &RayIntersection::material)
        .def_readwrite("texcoord", &RayIntersection::texcoord)
        .def_readwrite("hasTexCoord", &RayIntersection::hasTexCoord)
        // repr
        .def("__repr__", [](const RayIntersection & ri) {
                std::stringstream ss;
                ss << ri;
                return ss.str();
            });
        ;
}

