#include <iostream>
#include "constants.h"
#include "GradientEnvironmentMap.h"
#include "Ray.h"
#include "vectortypes.h"
#include "interpolation.h"
#include "rng.h"

GradientEnvironmentMap::GradientEnvironmentMap(const RadianceRGB & low,
                                               const RadianceRGB & high)
    : low(low), high(high)
{

}

GradientEnvironmentMap::GradientEnvironmentMap(const RadianceRGB & low,
                                               const RadianceRGB & high,
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

