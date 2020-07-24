#ifndef __LATLON_ENVIRONMENT_MAP_H__
#define __LATLON_ENVIRONMENT_MAP_H__

#include "EnvironmentMap.h"

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
