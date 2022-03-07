#ifndef __GRADIENT_ENVIRONMENT_MAP_H__
#define __GRADIENT_ENVIRONMENT_MAP_H__

#include "EnvironmentMap.h"

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


#endif
