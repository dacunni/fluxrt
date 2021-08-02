
void microfacet_bindings(py::module_ & m)
{
    // microfacet submodule
    auto m_microfacet = m.def_submodule("microfacet");

    // geometry submodule
    auto m_geometry = m_microfacet.def_submodule("geometry");

    m_geometry.def("implicit", &Microfacet::GeometryShadowing::Implicit);
    m_geometry.def("cook_torrance", &Microfacet::GeometryShadowing::CookTorrance);

    // normal submodule
    auto m_normal = m_microfacet.def_submodule("normal");

    m_normal.def("beckman", &Microfacet::NormalDistribution::Beckman);
    m_normal.def("blinn_phong", &Microfacet::NormalDistribution::BlinnPhong);
}

