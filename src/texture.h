#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <cstdint>
#include <limits>
#include <memory>
#include <map>

#include "image.h"

struct TextureCoordinate
{
    float u, v;
};

std::ostream & operator<<(std::ostream & os, const TextureCoordinate & texcoord);

using TextureID = uint32_t;

static const TextureID NoTexture = std::numeric_limits<TextureID>::max();

using Texture = Image<float>;
using TexturePtr = std::shared_ptr<Texture>;
using TextureArray = std::vector<TexturePtr>;

struct TextureCache
{
    TextureID loadTextureFromFile(const std::string & path,
                                  const std::string & filename);

    std::map<std::string, TextureID> fileToTextureID;
    TextureArray textures;
};


#endif
