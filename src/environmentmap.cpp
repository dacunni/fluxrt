#include "environmentmap.h"
#include "ray.h"

using namespace color;

ColorRGB EnvironmentMap::sampleRay(const Ray & ray)
{
    return ColorRGB::BLACK();
}

ColorRGB CubeMapEnvironmentMap::sampleRay(const Ray & ray)
{
    // TODO
    return ColorRGB::RED();
}


