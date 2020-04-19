#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include <cstdint>
#include <limits>
#include <memory>

#include "vectortypes.h"
#include "texture.h"
#include "radiometry.h"

struct ReflectanceRGB
{
    ReflectanceRGB(float r, float g, float b) : r(r), g(g), b(b) {}
    ReflectanceRGB(float rgb[3]) : r(rgb[0]), g(rgb[1]), b(rgb[2]) {}
    explicit ReflectanceRGB(const color::ColorRGB & c)
        : r(c.r), g(c.g), b(c.b) {}

    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;

    ReflectanceRGB residual() const { return { 1.0f - r, 1.0f - g, 1.0f - b }; }

    static const ReflectanceRGB RED() { return { 1.0f, 0.0f, 0.0f }; }
    static const ReflectanceRGB GREEN() { return { 0.0f, 1.0f, 0.0f }; }
    static const ReflectanceRGB BLUE() { return { 0.0f, 0.0f, 1.0f }; }
};

inline RadianceRGB operator*(const ReflectanceRGB & ref,
                             const RadianceRGB & rad);

struct ParameterRGB
{
    ParameterRGB(float r, float g, float b) : r(r), g(g), b(b) {}
    ParameterRGB(float rgb[3]) : r(rgb[0]), g(rgb[1]), b(rgb[2]) {}

    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
};

inline RadianceRGB operator*(const ParameterRGB & param,
                             const RadianceRGB & rad);
inline RadianceRGB operator*(const RadianceRGB & rad,
                             const ParameterRGB & param);

struct Medium {
    float indexOfRefraction = 1.0f;
    ParameterRGB beersLawAttenuation = { 0.0f, 0.0f, 0.0f };
};

const Medium VaccuumMedium = { 1.0f, { 0.0f, 0.0f, 0.0f } };

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
    TextureID normalMapTexture = NoTexture;

    // Refractive layer
    Medium innerMedium;
    bool isRefractive = false;

    inline ReflectanceRGB diffuse(const TextureArray & tex, const TextureCoordinate & texcoord) const;
    inline ReflectanceRGB specular(const TextureArray & tex, const TextureCoordinate & texcoord) const;
    inline bool hasDiffuse() const;
    inline bool hasSpecular() const;
    inline float alpha(const TextureArray & tex, const TextureCoordinate & texcoord) const;
    inline bool hasNormalMap() const;
    inline Direction3 normalMap(const TextureArray & tex, const TextureCoordinate & texcoord) const;

    // Factories
    static Material makeDiffuse(float D[3]);
    static Material makeDiffuse(const ReflectanceRGB & D);
    static Material makeDiffuseSpecular(float D[3], float S[3]);
    static Material makeDiffuseSpecular(const ReflectanceRGB & D, const ReflectanceRGB & S);
    static Material makeMirror();
    static Material makeRefractive(float ior);
};

static const Material DefaultMaterial = Material();

using MaterialArray = std::vector<Material>;

inline const Material & materialFromID(MaterialID id, MaterialArray & materials);

// Inline implementations

inline RadianceRGB operator*(const ReflectanceRGB & ref,
                             const RadianceRGB & rad) {
    return { ref.r * rad.r, ref.g * rad.g, ref.b * rad.b };
}

inline RadianceRGB operator*(const ParameterRGB & param,
                             const RadianceRGB & rad) {
    return { param.r * rad.r, param.g * rad.g, param.b * rad.b };
}

inline RadianceRGB operator*(const RadianceRGB & rad,
                             const ParameterRGB & param) {
    return operator*(param, rad);
}

inline ReflectanceRGB Material::diffuse(const TextureArray & tex, const TextureCoordinate & texcoord) const
{
    if(diffuseTexture != NoTexture) {
        auto & texture = tex[diffuseTexture];
        return ReflectanceRGB(texture->lerpUV3(texcoord.u, texcoord.v));
    }
    else {
        return diffuseColor;
    }
}

inline bool Material::hasDiffuse() const
{
    return diffuseTexture != NoTexture
        || diffuseColor.r > 0.0f
        || diffuseColor.g > 0.0f
        || diffuseColor.b > 0.0f;
}

inline ReflectanceRGB Material::specular(const TextureArray & tex, const TextureCoordinate & texcoord) const
{
    if(specularTexture != NoTexture) {
        auto & texture = tex[specularTexture];
        return ReflectanceRGB(texture->lerpUV3(texcoord.u, texcoord.v));
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

bool Material::hasNormalMap() const
{
    return normalMapTexture != NoTexture;
}

Direction3 Material::normalMap(const TextureArray & tex, const TextureCoordinate & texcoord) const
{
    if(normalMapTexture != NoTexture) {
        auto & texture = tex[normalMapTexture];
        float u = texcoord.u;
        float v = texcoord.v;
        return Direction3{ texture->lerpUV(u, v, 0),
                           texture->lerpUV(u, v, 1),
                           texture->lerpUV(u, v, 2) };
    }
    else {
        return Direction3{ 0.0f, 0.0f, 1.0f };
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
