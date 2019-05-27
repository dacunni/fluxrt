#ifndef __RAY_H__
#define __RAY_H__

#include <memory>
#include <limits>
#include <float.h>

#include "vectortypes.h"
#include "material.h"

struct Ray
{
    inline Ray() = default;
    inline Ray(const Position3 & o, const Direction3 & d) : origin(o), direction(d) {}
    inline ~Ray() = default;

    Position3 origin;
    Direction3 direction;
};

struct RayIntersection {
    inline RayIntersection() = default;
    inline ~RayIntersection() = default;

    Position3 position;
    Direction3 normal;
    Direction3 tangent;
    Direction3 bitangent;
    float distance = std::numeric_limits<float>::max();
    TextureCoordinate texcoord;
    bool hasTexCoord = false;
};

#endif
