#include "material.h"


Material Material::makeDiffuse(float D[3])
{
    Material m;
    m.diffuse[0] = D[0];
    m.diffuse[1] = D[1];
    m.diffuse[2] = D[2];
    return m;
}

Material Material::makeDiffuseSpecular(float D[3], float S[3])
{
    Material m;
    m.diffuse[0] = D[0];
    m.diffuse[1] = D[1];
    m.diffuse[2] = D[2];
    m.specular[0] = S[0];
    m.specular[1] = S[1];
    m.specular[2] = S[2];
    return m;
}

