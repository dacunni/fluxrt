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

        static std::string lookupPath(const std::string & fileName);
};


#endif
