#include <iostream>
#include "texture.h"
#include "Logger.h"

std::ostream & operator<<(std::ostream & os, const TextureCoordinate & texcoord)
{
    os << "TexCoord u " << texcoord.u << " v " << texcoord.v;
    return os;
}

TextureID TextureCache::loadTextureFromFile(const std::string & path,
                                            const std::string & filename)
{
    auto & logger = getLogger();

    // Fix separators to ensure Unix-style paths
    std::string texname = filename;
    std::replace(texname.begin(), texname.end(), '\\', '/');
    auto ft = fileToTextureID.find(texname);

    TextureID textureID = NoTexture;
    if(ft != fileToTextureID.end()) {
        // Texture is in the cache
        logger.debugf("Texture in cache '%s'", texname.c_str());
        textureID = ft->second;
    }
    else {
        // Cache miss. Load the texture and update the cache.
        textureID = textures.size();
        std::shared_ptr<Texture> texture;
        if(!path.empty()) {
            texname = path + '/' + texname;
        }
        logger.debugf("Reading image file '%s'", texname.c_str());
        texture = readImage<float>(texname);
        texture->outOfBoundsBehavior = Image<float>::Repeat;
        textures.push_back(texture);
        fileToTextureID[texname] = textureID;
    }

    return textureID;
}

