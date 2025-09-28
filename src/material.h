#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include <cstdint>
#include <limits>
#include <memory>

#include "vectortypes.h"
#include "texture.h"
#include "radiometry.h"
#include "ValueRGB.h"
#include "MaterialParameter.h"

class BRDF;

struct Medium {
    float indexOfRefraction = 1.0f;
    ParameterRGB beersLawAttenuation = { 0.0f, 0.0f, 0.0f };
};

const Medium VaccuumMedium = { 1.0f, { 0.0f, 0.0f, 0.0f } };

using MediumStack = std::vector<Medium>;

using MaterialID = uint32_t;

static const MaterialID NoMaterial = std::numeric_limits<MaterialID>::max();

struct Material
{
    // Diffuse reflectance
    MaterialParameterRGB diffuseParam = {{ 1.0f, 1.0f, 1.0f }};

    // Specular reflectance (all 0 indicates no specular present)
    MaterialParameterRGB specularParam = {{ 0.0f, 0.0f, 0.0f }};
    // Specular exponent (0 = mirror)
    MaterialParameterScalar specularExponentParam = { 0.0f };

    // Non-refractive opacity
    MaterialParameterScalar alphaParam = { 1.0f };
    float opacity = 1.0f;

    // Emission
    RadianceRGB emissionColor = { 0.0f, 0.0f, 0.0f };

    // Normal Map
    TextureID normalMapTexture = NoTexture;

    // Refractive layer
    Medium innerMedium;
    bool isRefractive = false;

    inline ReflectanceRGB diffuse(const TextureArray & tex, const TextureCoordinate & texcoord) const;
    inline ReflectanceRGB specular(const TextureArray & tex, const TextureCoordinate & texcoord) const;
    inline float specularExponent(const TextureArray & tex, const TextureCoordinate & texcoord) const;
    inline RadianceRGB emission(const TextureArray & tex, const TextureCoordinate & texcoord) const;
    inline Direction3 normalMap(const TextureArray & tex, const TextureCoordinate & texcoord) const;
    inline float alpha(const TextureArray & tex, const TextureCoordinate & texcoord) const;

    inline bool hasDiffuse() const;
    inline bool hasSpecular() const;
    inline bool hasEmission() const;
    inline bool hasNormalMap() const;

    inline bool isGlossy(const TextureArray & tex, const TextureCoordinate & texcoord) const
        { return specularExponent(tex, texcoord) > 0.01f; }

    // Apply normal map (if any) to the supplied basis vectors
    inline void applyNormalMap(const TextureArray & tex, const TextureCoordinate & texcoord,
                               Direction3 & normal, Direction3 & tangent, Direction3 & bitangent) const;

    void print() const;

    // Factories
    static Material makeDiffuse(float D[3]);
    static Material makeDiffuse(const ReflectanceRGB & D);
    static Material makeDiffuseSpecular(float D[3], float S[3]);
    static Material makeDiffuseSpecular(const ReflectanceRGB & D, const ReflectanceRGB & S);
    static Material makeMirror();
    static Material makeRefractive(float ior);
    static Material makeEmissive(float E[3]);
    static Material makeEmissive(const RadianceRGB & E);
};

static const Material DefaultMaterial = Material();

using MaterialArray = std::vector<Material>;

inline const Material & materialFromID(MaterialID id, const MaterialArray & materials);

// Inline implementations

inline ReflectanceRGB Material::diffuse(const TextureArray & tex, const TextureCoordinate & texcoord) const
{
    return diffuseParam.get(tex, texcoord);
}

inline bool Material::hasDiffuse() const
{
    return diffuseParam.isNonZero();
}

inline ReflectanceRGB Material::specular(const TextureArray & tex, const TextureCoordinate & texcoord) const
{
    return specularParam.get(tex, texcoord);
}

inline bool Material::hasSpecular() const
{
    return specularParam.isNonZero();
}

inline float Material::specularExponent(const TextureArray & tex, const TextureCoordinate & texcoord) const
{
    return specularExponentParam.get(tex, texcoord);
}

inline float Material::alpha(const TextureArray & tex, const TextureCoordinate & texcoord) const
{
    return opacity * alphaParam.get(tex, texcoord);
}

inline RadianceRGB Material::emission(const TextureArray & tex, const TextureCoordinate & texcoord) const
{
    // TODO - emissive texture
    return emissionColor;
}

inline bool Material::hasEmission() const
{
    // TODO - emissive texture
    return emissionColor.hasNonZeroComponent();
}

inline bool Material::hasNormalMap() const
{
    return normalMapTexture != NoTexture;
}

inline Direction3 Material::normalMap(const TextureArray & tex, const TextureCoordinate & texcoord) const
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

inline void Material::applyNormalMap(const TextureArray & tex, const TextureCoordinate & texcoord,
                                     Direction3 & normal, Direction3 & tangent, Direction3 & bitangent) const
{
    if(!hasNormalMap()) {
        return;
    }

    auto map = normalMap(tex, texcoord);

    // Compute new basis by purturbing via the normal map
    auto N = (normal * map.z + tangent * map.x + bitangent * map.y).normalized();
    auto T = cross(bitangent, N).normalized();
    auto B = cross(N, T).normalized();

    // Copy new values to output
    normal    = N;
    tangent   = T;
    bitangent = B;
}

inline const Material & materialFromID(MaterialID id, const MaterialArray & materials)
{
    if(id == NoMaterial) {
        return DefaultMaterial;
    }
    else {
        assert(id < materials.size());
        return materials[id];
    }
}

#endif
