#ifndef __RAY_H__
#define __RAY_H__

#include <memory>
#include <limits>
#include <iosfwd>
#include <float.h>

#include "vectortypes.h"
#include "material.h"

struct Ray
{
    inline Ray() = default;
    inline Ray(const Position3 & o, const Direction3 & d) : origin(o), direction(d) {}
    inline ~Ray() = default;

    // Get a point at signed distance t from the origin along the ray
    Position3 pointAt(float t) const { return origin + direction * t; }

    Position3 origin;
    Direction3 direction;
};

struct RayIntersection
{
    inline RayIntersection() = default;
    inline ~RayIntersection() = default;

    Ray ray;
    Position3 position;
    Direction3 normal;
    Direction3 tangent;
    Direction3 bitangent;
    float distance = std::numeric_limits<float>::max();
    MaterialID material = NoMaterial;
    TextureCoordinate texcoord;
    bool hasTexCoord = false;
};

std::ostream & operator<<(std::ostream & os, const Ray & r);
std::ostream & operator<<(std::ostream & os, const RayIntersection & ri);

#endif
