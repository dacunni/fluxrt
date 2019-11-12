#include "material.h"

Material Material::makeDiffuse(float D[3])
{
    Material m;
    m.diffuseColor = ReflectanceRGB(D);
    return m;
}

Material Material::makeDiffuseSpecular(float D[3], float S[3])
{
    Material m;
    m.diffuseColor  = ReflectanceRGB(D);
    m.specularColor = ReflectanceRGB(S);
    return m;
}

