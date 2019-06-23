#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include <cstdint>
#include <limits>
#include <memory>

#include "image.h"

struct TextureCoordinate
{
    float u, v;
};

struct ReflectanceRGB
{
    float r, g, b;

    static const ReflectanceRGB RED() { return { 1.0f, 0.0f, 0.0f }; }
};

using MaterialID = uint32_t;
using TextureID = uint32_t;

static const MaterialID NoMaterial = std::numeric_limits<MaterialID>::max();
static const TextureID NoTexture = std::numeric_limits<TextureID>::max();

using Texture = Image<float>;
using TextureArray = std::vector<std::shared_ptr<Texture>>;

struct Material
{
    ReflectanceRGB diffuseColor =  { 1.0f, 1.0f, 1.0f };
    ReflectanceRGB specularColor = { 0.0f, 0.0f, 0.0f };

    TextureID diffuseTexture = NoTexture;
    TextureID specularTexture = NoTexture;

    inline ReflectanceRGB diffuse(const TextureArray & tex, const TextureCoordinate & texcoord) const;
    inline ReflectanceRGB specular(const TextureArray & tex, const TextureCoordinate & texcoord) const;

    // Factories
    static Material makeDiffuse(float D[3]);
    static Material makeDiffuseSpecular(float D[3], float S[3]);
};

using MaterialArray = std::vector<Material>;

// Inline implementations

inline ReflectanceRGB Material::diffuse(const TextureArray & tex, const TextureCoordinate & texcoord) const
{
    if(diffuseTexture != NoTexture) {
        auto & texture = tex[diffuseTexture];
        float u = texcoord.u;
        float v = texcoord.v;
        return { texture->lerpUV(u, v, 0),
                 texture->lerpUV(u, v, 1),
                 texture->lerpUV(u, v, 2) };
    }
    else {
        return diffuseColor;
    }
}

inline ReflectanceRGB Material::specular(const TextureArray & tex, const TextureCoordinate & texcoord) const
{
    return specularColor;
}

#endif
