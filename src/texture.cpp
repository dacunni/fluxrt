#include <iostream>
#include "texture.h"

std::ostream & operator<<(std::ostream & os, const TextureCoordinate & texcoord)
{
    os << "TexCoord u " << texcoord.u << " v " << texcoord.v;
    return os;
}

TextureID TextureCache::loadTextureFromFile(const std::string & path,
                                            const std::string & filename)
{
    // Fix separators to ensure Unix-style paths
    std::string texname = filename;
    std::replace(texname.begin(), texname.end(), '\\', '/');
    auto ft = fileToTextureID.find(texname);

    TextureID textureID = NoTexture;
    if(ft != fileToTextureID.end()) {
        // Texture is in the cache
        textureID = ft->second;
    }
    else {
        // Cache miss. Load the texture and update the cache.
        textureID = textures.size();
        std::shared_ptr<Texture> texture;
        if(!path.empty()) {
            texname = path + '/' + texname;
        }
        texture = readImage<float>(texname);
        texture->outOfBoundsBehavior = Image<float>::Repeat;
        textures.push_back(texture);
        fileToTextureID[texname] = textureID;
    }

    return textureID;
}

