#ifndef __ENVIRONMENT_MAP_H__
#define __ENVIRONMENT_MAP_H__

#include "color.h"

class Ray;

class EnvironmentMap
{
    public:
        EnvironmentMap() = default;
        virtual ~EnvironmentMap() = default;

        virtual color::ColorRGB sampleRay(const Ray & ray);
};

class CubeMapEnvironmentMap
{
    public:
        CubeMapEnvironmentMap() = default;
        virtual ~CubeMapEnvironmentMap() = default;


        virtual color::ColorRGB sampleRay(const Ray & ray);
};

#endif
