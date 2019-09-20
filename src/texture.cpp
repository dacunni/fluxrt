#include "texture.h"

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
        texture = readImage<float>(path + '/' + texname);
        texture->outOfBoundsBehavior = Image<float>::Repeat;
        textures.push_back(texture);
        fileToTextureID[texname] = textureID;
    }

    return textureID;
}

