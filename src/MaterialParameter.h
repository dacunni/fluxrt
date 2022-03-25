#ifndef __MATERIAL_PARAMETER_H__
#define __MATERIAL_PARAMETER_H__

#include <cstdint>
#include <limits>
#include <memory>

#include "vectortypes.h"
#include "texture.h"
#include "radiometry.h"
#include "ValueRGB.h"

struct MaterialParameterRGB
{
    MaterialParameterRGB() = default;
    MaterialParameterRGB(const ReflectanceRGB & u) : uniform(u) {}
    MaterialParameterRGB(TextureID id) : textureId(id) {}

    inline ReflectanceRGB get(const TextureArray & tex, const TextureCoordinate & texcoord) const
    {
        if(textureId != NoTexture) {
            auto & texture = tex[textureId];
            return ReflectanceRGB(texture->lerpUV3(texcoord.u, texcoord.v));
        }
        else {
            return uniform;
        }
    }

    inline bool isNonZero() const
    {
        return textureId != NoTexture || uniform.hasNonZeroComponent();
    }

    ReflectanceRGB uniform = { 0.0f, 0.0f, 0.0f };
    TextureID textureId = NoTexture;
};

struct MaterialParameterScalar
{
    MaterialParameterScalar() = default;
    MaterialParameterScalar(float u) : uniform(u) {}
    MaterialParameterScalar(TextureID id) : textureId(id) {}

    inline float get(const TextureArray & tex, const TextureCoordinate & texcoord) const
    {
        if(textureId != NoTexture) {
            auto & texture = tex[textureId];
            // Take the last channel, assuming 1 channel is a mask, 3 is B&W, and 4 has alpha
            return texture->lerpUV(texcoord.u, texcoord.v, texture->numChannels - 1);
        }
        else {
            return uniform;
        }
    }

    inline bool isNonZero() const
    {
        return textureId != NoTexture || uniform != 0.0f;
    }

    float uniform = 0.0f;
    TextureID textureId = NoTexture;
};

#endif
