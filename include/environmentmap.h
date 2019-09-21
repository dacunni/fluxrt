#ifndef __ENVIRONMENT_MAP_H__
#define __ENVIRONMENT_MAP_H__

#include "color.h"
#include "texture.h"

class Ray;
class Direction3;

class EnvironmentMap
{
    public:
        EnvironmentMap() = default;
        virtual ~EnvironmentMap() = default;

        virtual color::ColorRGB sampleRay(const Ray & ray);
};

class GradientEnvironmentMap : public EnvironmentMap
{
    public:
        GradientEnvironmentMap(const color::ColorRGB & low,
                               const color::ColorRGB & high);

        virtual color::ColorRGB sampleRay(const Ray & ray);

    protected:
        color::ColorRGB low;
        color::ColorRGB high;
};

class CubeMapEnvironmentMap : public EnvironmentMap
{
    public:
        CubeMapEnvironmentMap() = default;
        virtual ~CubeMapEnvironmentMap() = default;

        bool loadFromDirectionFiles(
            const std::string & negx,
            const std::string & posx,
            const std::string & negy,
            const std::string & posy,
            const std::string & negz,
            const std::string & posz);

        virtual color::ColorRGB sampleRay(const Ray & ray);

    protected:
        using TexturePtr = std::shared_ptr<Texture>;

        TexturePtr loadDirectionTile(const std::string & file);

        void directionToTileCoord(const Direction3 & v,
                                  TexturePtr & texture,
                                  TextureCoordinate & texcoord);

        TexturePtr xn;
        TexturePtr xp;
        TexturePtr yn;
        TexturePtr yp;
        TexturePtr zn;
        TexturePtr zp;
};

#endif
