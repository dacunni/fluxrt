
void fresnel_bindings(py::module_ & m)
{
    // fresnel submodule
    auto m_fresnel = m.def_submodule("fresnel");

    // fresnel free functions
    m_fresnel.def("schlick", static_cast<float(*)(float, float)>(&fresnel::schlick));

    // dialectric submodule
    auto m_dialectric = m_fresnel.def_submodule("dialectric");

    m_dialectric.def("unpolarized", &fresnel::dialectric::unpolarized);
    m_dialectric.def("parallel", &fresnel::dialectric::parallel);
    m_dialectric.def("perpendicular", &fresnel::dialectric::perpendicular);
    m_dialectric.def("unpolarizedSnell", &fresnel::dialectric::unpolarizedSnell);
    m_dialectric.def("parallelSnell", &fresnel::dialectric::parallelSnell);
    m_dialectric.def("perpendicularSnell", &fresnel::dialectric::perpendicularSnell);
    m_dialectric.def("normal", &fresnel::dialectric::normal);

    // conductor submodule
    auto m_conductor = m_fresnel.def_submodule("conductor");

    m_conductor.def("unpolarized", &fresnel::conductor::unpolarized);
    m_conductor.def("normal", &fresnel::conductor::normal);
}

