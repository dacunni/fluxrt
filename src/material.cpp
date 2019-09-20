#include "material.h"

Material Material::makeDiffuse(float D[3])
{
    Material m;
    m.diffuseColor.r = D[0];
    m.diffuseColor.g = D[1];
    m.diffuseColor.b = D[2];
    return m;
}

Material Material::makeDiffuseSpecular(float D[3], float S[3])
{
    Material m;
    m.diffuseColor.r  = D[0];
    m.diffuseColor.g  = D[1];
    m.diffuseColor.b  = D[2];
    m.specularColor.r = S[0];
    m.specularColor.g = S[1];
    m.specularColor.b = S[2];
    return m;
}

