#ifndef __ENVIRONMENT_MAP_H__
#define __ENVIRONMENT_MAP_H__

#include "texture.h"
#include "radiometry.h"
#include "vectortypes.h"
#include "vec2.h"

class Ray;
class RNG;

struct RandomDirection {
    Direction3 direction;
    float pdf;
};

class EnvironmentMap
{
    public:
        EnvironmentMap() = default;
        virtual ~EnvironmentMap() = default;

        virtual RadianceRGB sampleRay(const Ray & ray);

        // Importance sampling
        virtual vec2 importanceSample(float e1, float e2, float & pdf) const { pdf = 0.0f; return vec2(0.0f, 0.0f); }
        virtual RandomDirection importanceSampleDirection(float e1, float e2) const { return { Direction3(0.0f, 0.0f, 0.0f), 0.0f }; }
        virtual bool canImportanceSample() const { return false; }

        virtual void saveDebugImages() {};
        virtual void saveDebugSampleImage() {};
};

class GradientEnvironmentMap : public EnvironmentMap
{
    public:
        GradientEnvironmentMap(const RadianceRGB & low,
                               const RadianceRGB & high);
        GradientEnvironmentMap(const RadianceRGB & low,
                               const RadianceRGB & high,
                               const Direction3 & direction);

        RadianceRGB sampleRay(const Ray & ray) override;

    protected:
        RadianceRGB low;
        RadianceRGB high;
        Direction3 direction{ 0.0f, 1.0f, 0.0f };
};

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

class LatLonEnvironmentMap : public EnvironmentMap
{
    public:
        LatLonEnvironmentMap() = default;
        virtual ~LatLonEnvironmentMap() = default;

        void loadFromFile(const std::string & filename);
        void loadFromImage(const Image<float> & image);

        RadianceRGB sampleRay(const Ray & ray) override;

        void setScaleFactor(float f) { scaleFactor = f; }

        TexturePtr getTexture() const { return texture; }

        // Importance sample using index variables e1,e2 in [0, 1]
        // Returns pixel coordinate of index
        vec2 importanceSample(float e1, float e2, float & pdf) const override;
        RandomDirection importanceSampleDirection(float e1, float e2) const override;
        bool canImportanceSample() const override { return true; }

        void saveDebugImages() override;
        void saveDebugSampleImage() override;

    protected:
        void buildImportanceSampleLookup();

        TexturePtr texture;

        // Importance sampling
        //   PDFs/CDFs stored per row in textures
        //   PDF/CDF of row aggregates
        TexturePtr         rowSums; // Cumulative sums along rows
        std::vector<float> cumRows; // Cumulative sum of row sums
        TexturePtr         pdf2D;   // PDF

        float scaleFactor = 1.0f;

        // Debug
        TexturePtr debugSamples;
};


#endif
