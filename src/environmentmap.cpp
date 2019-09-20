#include <iostream>
#include "environmentmap.h"
#include "ray.h"
#include "vectortypes.h"

using namespace color;

ColorRGB EnvironmentMap::sampleRay(const Ray & ray)
{
    return ColorRGB::BLACK();
}

CubeMapEnvironmentMap::TexturePtr CubeMapEnvironmentMap::loadDirectionTile(const std::string & filename)
{
    auto texture = readImage<float>(filename);
    if(!texture) {
        throw 0;
    }

    texture->outOfBoundsBehavior = Texture::Repeat;

    return texture;
}

bool CubeMapEnvironmentMap::loadFromDirectionFiles(
    const std::string & xnFile,
    const std::string & xpFile,
    const std::string & ynFile,
    const std::string & ypFile,
    const std::string & znFile,
    const std::string & zpFile)
{
    try {
        xn = loadDirectionTile(xnFile);
        xp = loadDirectionTile(xpFile);
        yn = loadDirectionTile(ynFile);
        yp = loadDirectionTile(ypFile);
        zn = loadDirectionTile(znFile);
        zp = loadDirectionTile(zpFile);
        return true;
    }
    catch(std::exception & e) {
        std::cout << "Caught exception: " << e.what() << std::endl;
        return false;
    }
    catch(...) {
        std::cout << "Caught unknown exception" << std::endl;
        return false;
    }
}

void CubeMapEnvironmentMap::directionToTileCoord(const Direction3 & v,
                                                 TexturePtr & texture,
                                                 TextureCoordinate & texcoord)
{
    float absX = std::abs(v.x);
    float absY = std::abs(v.y);
    float absZ = std::abs(v.z);

    bool isXPositive = v.x > 0;
    bool isYPositive = v.y > 0;
    bool isZPositive = v.z > 0;

    float maxAxis, uc, vc;

    if (!isXPositive && absX >= absY && absX >= absZ) {
        // u (0 to 1) goes from -z to +z
        // v (0 to 1) goes from -y to +y
        maxAxis = absX;
        uc = v.z;
        vc = v.y;
        texture = xn;
    }
    if (isXPositive && absX >= absY && absX >= absZ) {
        // u (0 to 1) goes from +z to -z
        // v (0 to 1) goes from -y to +y
        maxAxis = absX;
        uc = -v.z;
        vc = v.y;
        texture = xp;
    }
    if (!isYPositive && absY >= absX && absY >= absZ) {
        // u (0 to 1) goes from -x to +x
        // v (0 to 1) goes from -z to +z
        maxAxis = absY;
        uc = v.x;
        vc = v.z;
        texture = yn;
    }
    if (isYPositive && absY >= absX && absY >= absZ) {
        // u (0 to 1) goes from -x to +x
        // v (0 to 1) goes from +z to -z
        maxAxis = absY;
        uc = v.x;
        vc = -v.z;
        texture = yp;
    }
    if (!isZPositive && absZ >= absX && absZ >= absY) {
        // u (0 to 1) goes from +x to -x
        // v (0 to 1) goes from -y to +y
        maxAxis = absZ;
        uc = -v.x;
        vc = v.y;
        texture = zn;
    }
    if (isZPositive && absZ >= absX && absZ >= absY) {
        // u (0 to 1) goes from -x to +x
        // v (0 to 1) goes from -y to +y
        maxAxis = absZ;
        uc = v.x;
        vc = v.y;
        texture = zp;
    }

    // Convert range from -1 to 1 to 0 to 1
    texcoord.u = 0.5f * (uc / maxAxis + 1.0f);
    texcoord.v = 0.5f * (vc / maxAxis + 1.0f);

    // V runs bottom up in cube maps
    texcoord.v = -texcoord.v;
}

ColorRGB CubeMapEnvironmentMap::sampleRay(const Ray & ray)
{
    TexturePtr texture;
    TextureCoordinate texcoord;

    directionToTileCoord(ray.direction, texture, texcoord);

    //std::cout << "tex " << texcoord.u << ", " << texcoord.v << '\n'; // TEMP

    return { texture->lerpUV(texcoord.u, texcoord.v, 0),
             texture->lerpUV(texcoord.u, texcoord.v, 1),
             texture->lerpUV(texcoord.u, texcoord.v, 2) };

    // TODO
    //return ColorRGB::RED();
}


