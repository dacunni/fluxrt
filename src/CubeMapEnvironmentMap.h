#ifndef __CUBEMAP_ENVIRONMENT_MAP_H__
#define __CUBEMAP_ENVIRONMENT_MAP_H__

#include "EnvironmentMap.h"

class CubeMapEnvironmentMap : public EnvironmentMap
{
    public:
        CubeMapEnvironmentMap() = default;
        virtual ~CubeMapEnvironmentMap() = default;

        void loadFromDirectionFiles(const std::string & negx,
                                    const std::string & posx,
                                    const std::string & negy,
                                    const std::string & posy,
                                    const std::string & negz,
                                    const std::string & posz);

        RadianceRGB sampleRay(const Ray & ray) override;

        void setScaleFactor(float f) { scaleFactor = f; }

    protected:
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

        float scaleFactor = 1.0f;
};


#endif
