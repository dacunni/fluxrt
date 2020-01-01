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

