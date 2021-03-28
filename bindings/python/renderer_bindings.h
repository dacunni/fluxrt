
void renderer_bindings(py::module_ & m)
{
    py::class_<Renderer, std::shared_ptr<Renderer>>(m, "Renderer")
        // constructors
        .def(py::init<>())
        // methods
        .def("traceRay", static_cast<
             bool (Renderer::*)(const Scene &, RNG &,
                                const Ray &,
                                const float,
                                const unsigned int,
                                const MediumStack &,
                                bool,
                                bool,
                                RayIntersection &,
                                RadianceRGB &) const>(&Renderer::traceRay))
        .def("traceRay", static_cast<
             RadianceRGB (Renderer::*) (const Scene &, RNG &,
                                        const Ray &,
                                        const float,
                                        const unsigned int,
                                        const MediumStack &,
                                        bool,
                                        bool) const>(&Renderer::traceRay))
        .def("traceCameraRay", static_cast<
             bool (Renderer::*)(const Scene &, RNG &,
                                const Ray &,
                                const float,
                                const unsigned int,
                                const MediumStack &,
                                RayIntersection &,
                                RadianceRGB &) const>(&Renderer::traceCameraRay))

        .def("printConfiguration", &Renderer::printConfiguration,
             py::call_guard<py::scoped_ostream_redirect>())
        // properties
        .def_readwrite("epsilon", &Renderer::epsilon)
        .def_readwrite("maxDepth", &Renderer::maxDepth)
        .def_readwrite("monteCarloRefraction", &Renderer::monteCarloRefraction)
        .def_readwrite("russianRouletteChance", &Renderer::russianRouletteChance)
        .def_readwrite("russianRouletteMinDepth", &Renderer::russianRouletteMinDepth)
        .def_readwrite("shadeDiffuseParams", &Renderer::shadeDiffuseParams)
        .def_readwrite("shadeSpecularParams", &Renderer::shadeSpecularParams)
        ;

    py::class_<Renderer::DiffuseShadingParameters,
               std::shared_ptr<Renderer::DiffuseShadingParameters>>(m, "DiffuseShadingParameters")
        // constructors
        .def(py::init<>())
        .def_readwrite("numEnvMapSamples", &Renderer::DiffuseShadingParameters::numEnvMapSamples)
        .def_readwrite("sampleCosineLobe", &Renderer::DiffuseShadingParameters::sampleCosineLobe)
        .def_readwrite("sampleLights", &Renderer::DiffuseShadingParameters::sampleLights)
        ;

    py::class_<Renderer::SpecularShadingParameters,
               std::shared_ptr<Renderer::SpecularShadingParameters>>(m , "SpecularShadingParameters")
        // constructors
        .def(py::init<>())
        .def_readwrite("samplePhongLobe", &Renderer::SpecularShadingParameters::samplePhongLobe)
        ;
}

