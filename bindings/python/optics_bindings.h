
void optics_bindings(py::module_ & m)
{
    // optics submodule
    auto m_optics = m.def_submodule("optics");

    // optics free functions
    m_optics.def("snellsLawAngle", &optics::snellsLawAngle);
    m_optics.def("snellsLawSine", &optics::snellsLawSine);
    m_optics.def("beersLawAttenuation", &optics::beersLawAttenuation);

    // ior submodule
    auto m_ior = m_optics.def_submodule("ior");

    m_ior.def("sellmeier", &optics::ior::sellmeier);
}

