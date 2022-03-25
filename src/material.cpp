#include <cstdio>
#include "material.h"

void Material::print() const
{
    std::string has;
    if(hasDiffuse())   has += "D";
    if(hasSpecular())  has += "S";
    if(hasEmission())  has += "E";
    if(hasNormalMap()) has += "N";
    printf("Material comp=%s\n", has.c_str());
}

Material Material::makeDiffuse(float D[3])
{
    Material m;
    m.diffuseParam = ReflectanceRGB(D);
    return m;
}

Material Material::makeDiffuse(const ReflectanceRGB & D)
{
    Material m;
    m.diffuseParam = D;
    return m;
}

Material Material::makeDiffuseSpecular(float D[3], float S[3])
{
    Material m;
    m.diffuseParam = ReflectanceRGB(D);
    m.specularParam = ReflectanceRGB(S);
    return m;
}

Material Material::makeDiffuseSpecular(const ReflectanceRGB & D, const ReflectanceRGB & S)
{
    Material m;
    m.diffuseParam  = D;
    m.specularParam = S;
    return m;
}

Material Material::makeMirror()
{
    Material m;
    m.diffuseParam  = ReflectanceRGB(0.0f, 0.0f, 0.0f);
    m.specularParam = ReflectanceRGB(1.0f, 1.0f, 1.0f);
    return m;
}

Material Material::makeRefractive(float ior)
{
    Material m;
    m.innerMedium.indexOfRefraction = ior;
    m.isRefractive = true;
    return m;
}

Material Material::makeEmissive(float E[3])
{
    Material m;
    m.diffuseParam = ReflectanceRGB{ 0.0f, 0.0f, 0.0f };
    m.emissionColor = RadianceRGB(E);
    return m;
}

Material Material::makeEmissive(const RadianceRGB & E)
{
    Material m;
    m.diffuseParam = ReflectanceRGB{ 0.0f, 0.0f, 0.0f };
    m.emissionColor = E;
    return m;
}

