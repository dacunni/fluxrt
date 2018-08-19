#ifndef __RAY_H__
#define __RAY_H__

#include <memory>
#include <limits>
#include <float.h>

#include "vec3.h"

struct Ray
{
    inline Ray() = default;
    inline Ray(const Position3 & o, const Direction3 & d) : origin(o), direction(d) {}
    inline ~Ray() = default;

    Position3 origin;
    Direction3 direction;
};

struct Sphere
{
	inline Sphere() = default;
	inline Sphere(const Position3 & c, float r) : center(c), radius(r) {}
	inline ~Sphere() = default;

	Position3 center;
	float radius;
};

struct RayIntersection {
    inline RayIntersection() = default;
    inline ~RayIntersection() = default;

    Position3 position;
    Direction3 normal;
    float distance = std::numeric_limits<float>::max();
};


inline bool intersects(const Ray & ray, const Sphere & sphere, float minDistance);

inline bool findIntersection(const Ray & ray, const Sphere & sphere, float minDistance, RayIntersection & intersection);

#include "raysphere.hpp"

#endif
