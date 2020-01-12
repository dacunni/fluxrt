#include "material.h"

Material Material::makeDiffuse(float D[3])
{
    Material m;
    m.diffuseColor = ReflectanceRGB(D);
    return m;
}

Material Material::makeDiffuse(const ReflectanceRGB & D)
{
    Material m;
    m.diffuseColor = D;
    return m;
}

Material Material::makeDiffuseSpecular(float D[3], float S[3])
{
    Material m;
    m.diffuseColor  = ReflectanceRGB(D);
    m.specularColor = ReflectanceRGB(S);
    return m;
}

Material Material::makeDiffuseSpecular(const ReflectanceRGB & D, const ReflectanceRGB & S)
{
    Material m;
    m.diffuseColor  = D;
    m.specularColor = S;
    return m;
}

Material Material::makeMirror()
{
    Material m;
    m.diffuseColor  = ReflectanceRGB(0.0f, 0.0f, 0.0f);
    m.specularColor = ReflectanceRGB(1.0f, 1.0f, 1.0f);
    return m;
}

Material Material::makeRefractive(float ior)
{
    Material m;
    m.indexOfRefraction = ior;
    m.isRefractive = true;
    return m;
}

