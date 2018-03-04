#ifndef __RAY_H__
#define __RAY_H__

#include <memory>
#include <float.h>

#include "vec3.h"

struct Ray
{
    inline Ray() {}
    inline Ray(const Position3 & o, const Direction3 & d) : origin(o), direction(d) {}
    inline ~Ray() {}

    Position3 origin;
    Direction3 direction;
};

struct Sphere
{
	inline Sphere() {}
	inline Sphere(const Position3 & c, float r) : center(c), radius(r) {}
	inline ~Sphere() {}

	Position3 center;
	float radius;
};

struct RayIntersection {
    inline RayIntersection() : distance(FLT_MAX) {}
    inline ~RayIntersection() {}

    Position3 position;
    Direction3 normal;
    float distance;
};


inline bool intersects(const Ray & ray, const Sphere & sphere, float minDistance);

inline bool findIntersection(const Ray & ray, const Sphere & sphere, float minDistance, RayIntersection & intersection);

#include "raysphere.hpp"

#endif
