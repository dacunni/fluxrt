#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include <cstdint>
#include <limits>
#include <memory>
#include <map>

#include "texture.h"
#include "radiometry.h"

struct ReflectanceRGB
{
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;

    static const ReflectanceRGB RED() { return { 1.0f, 0.0f, 0.0f }; }
    static const ReflectanceRGB GREEN() { return { 0.0f, 1.0f, 0.0f }; }
    static const ReflectanceRGB BLUE() { return { 0.0f, 0.0f, 1.0f }; }
};

inline radiometry::RadianceRGB operator*(const ReflectanceRGB & ref,
                                         const radiometry::RadianceRGB & rad);

using MaterialID = uint32_t;

static const MaterialID NoMaterial = std::numeric_limits<MaterialID>::max();

struct Material
{
    ReflectanceRGB diffuseColor =  { 1.0f, 1.0f, 1.0f };
    // Specular of all 0 indicates no specular present
    ReflectanceRGB specularColor = { 0.0f, 0.0f, 0.0f };

    TextureID diffuseTexture  = NoTexture;
    TextureID specularTexture = NoTexture;
    TextureID alphaTexture    = NoTexture;

    inline ReflectanceRGB diffuse(const TextureArray & tex, const TextureCoordinate & texcoord) const;
    inline ReflectanceRGB specular(const TextureArray & tex, const TextureCoordinate & texcoord) const;
    inline bool hasSpecular() const;
    inline float alpha(const TextureArray & tex, const TextureCoordinate & texcoord) const;

    // Factories
    static Material makeDiffuse(float D[3]);
    static Material makeDiffuseSpecular(float D[3], float S[3]);
};

static const Material DefaultMaterial = Material();

using MaterialArray = std::vector<Material>;

inline const Material & materialFromID(MaterialID id, MaterialArray & materials);

// Inline implementations

inline radiometry::RadianceRGB operator*(const ReflectanceRGB & ref,
                                         const radiometry::RadianceRGB & rad) {
    return { ref.r * rad.r, ref.g * rad.g, ref.b * rad.b };
}

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
    if(specularTexture != NoTexture) {
        auto & texture = tex[specularTexture];
        float u = texcoord.u;
        float v = texcoord.v;
        return { texture->lerpUV(u, v, 0),
                 texture->lerpUV(u, v, 1),
                 texture->lerpUV(u, v, 2) };
    }
    else {
        return specularColor;
    }
}

inline bool Material::hasSpecular() const
{
    return specularTexture != NoTexture
        || specularColor.r > 0.0f
        || specularColor.g > 0.0f
        || specularColor.b > 0.0f;
}

float Material::alpha(const TextureArray & tex, const TextureCoordinate & texcoord) const
{
    if(alphaTexture != NoTexture) {
        auto & texture = tex[alphaTexture];
        float u = texcoord.u;
        float v = texcoord.v;
        // Take the last channel, assuming 1 channel is a mask, 3 is B&W, and 4 has alpha
        return texture->lerpUV(u, v, texture->numChannels - 1);
    }
    else {
        return 1.0f;
    }
}

inline const Material & materialFromID(MaterialID id, const MaterialArray & materials)
{
    if(id == NoMaterial) {
        return DefaultMaterial;
    }
    else {
        return materials[id];
    }
}

#endif
