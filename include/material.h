#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include <cstdint>
#include <limits>

struct TextureCoordinate
{
    float u, v;
};

struct Material
{
    float diffuse[3] =  { 1.0f, 1.0f, 1.0f };
    float specular[3] = { 0.0f, 0.0f, 0.0f };

    // Factories
    static Material makeDiffuse(float D[3]);
    static Material makeDiffuseSpecular(float D[3], float S[3]);
};

using MaterialID = uint32_t;

static const MaterialID NoMaterial = std::numeric_limits<MaterialID>::max();

#endif
