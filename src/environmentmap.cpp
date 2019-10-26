#include <iostream>
#include "environmentmap.h"
#include "ray.h"
#include "vectortypes.h"
#include "interpolation.h"

using namespace radiometry;

RadianceRGB EnvironmentMap::sampleRay(const Ray & ray)
{
    return RadianceRGB();
}

GradientEnvironmentMap::GradientEnvironmentMap(const RadianceRGB & low,
                                               const RadianceRGB & high)
    : low(low), high(high)
{

}

GradientEnvironmentMap::GradientEnvironmentMap(const radiometry::RadianceRGB & low,
                                               const radiometry::RadianceRGB & high,
                                               const Direction3 & direction)
    : low(low), high(high), direction(direction.normalized())
{

}

RadianceRGB GradientEnvironmentMap::sampleRay(const Ray & ray)
{
    // [-1, 1] assuming ray.direction is normalized
    float a = dot(ray.direction, direction);
    return {
        lerpFromTo(a, -1.0f, 1.0f, low.r, high.r),
        lerpFromTo(a, -1.0f, 1.0f, low.g, high.g),
        lerpFromTo(a, -1.0f, 1.0f, low.b, high.b)
    };
}

CubeMapEnvironmentMap::TexturePtr CubeMapEnvironmentMap::loadDirectionTile(const std::string & filename)
{
    auto rawTexture = readImage<float>(filename);
    if(!rawTexture) {
        throw 0;
    }

    // FIXME: This is making excessive copies
    TexturePtr texture = std::make_shared<Texture>(rawTexture->width, rawTexture->height, rawTexture->numChannels);
    *texture = applyInverseStandardGamma(*rawTexture);

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

// Reference: https://en.wikipedia.org/wiki/Cube_mapping
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

    float maxAxis;

    if(!isXPositive && absX >= absY && absX >= absZ) {
        // u (0 to 1) goes from -z to +z
        // v (0 to 1) goes from -y to +y
        maxAxis = absX;
        texcoord.u = v.z;
        texcoord.v = v.y;
        texture = xn;
    }
    else if(isXPositive && absX >= absY && absX >= absZ) {
        // u (0 to 1) goes from +z to -z
        // v (0 to 1) goes from -y to +y
        maxAxis = absX;
        texcoord.u = -v.z;
        texcoord.v = v.y;
        texture = xp;
    }
    else if(!isYPositive && absY >= absX && absY >= absZ) {
        // u (0 to 1) goes from -x to +x
        // v (0 to 1) goes from -z to +z
        maxAxis = absY;
        texcoord.u = v.x;
        texcoord.v = v.z;
        texture = yn;
    }
    else if(isYPositive && absY >= absX && absY >= absZ) {
        // u (0 to 1) goes from -x to +x
        // v (0 to 1) goes from +z to -z
        maxAxis = absY;
        texcoord.u = v.x;
        texcoord.v = -v.z;
        texture = yp;
    }
    else if(!isZPositive && absZ >= absX && absZ >= absY) {
        // u (0 to 1) goes from +x to -x
        // v (0 to 1) goes from -y to +y
        maxAxis = absZ;
        texcoord.u = -v.x;
        texcoord.v = v.y;
        texture = zn;
    }
    else { // default : if(isZPositive && absZ >= absX && absZ >= absY) {
        // u (0 to 1) goes from -x to +x
        // v (0 to 1) goes from -y to +y
        maxAxis = absZ;
        texcoord.u = v.x;
        texcoord.v = v.y;
        texture = zp;
    }

    // Convert range from -1 to 1 to 0 to 1
    texcoord.u = 0.5f * (texcoord.u / maxAxis + 1.0f);
    texcoord.v = 0.5f * (texcoord.v / maxAxis + 1.0f);

    // V runs bottom up in cube maps
    texcoord.v = -texcoord.v;
}

RadianceRGB CubeMapEnvironmentMap::sampleRay(const Ray & ray)
{
    TexturePtr texture;
    TextureCoordinate texcoord;

    directionToTileCoord(ray.direction, texture, texcoord);

    //std::cout << "tex " << texcoord.u << ", " << texcoord.v << '\n'; // TEMP

    return { scaleFactor * texture->lerpUV(texcoord.u, texcoord.v, 0),
             scaleFactor * texture->lerpUV(texcoord.u, texcoord.v, 1),
             scaleFactor * texture->lerpUV(texcoord.u, texcoord.v, 2) };

    // TODO
    //return RadianceRGB::RED();
}


