#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "ray.h"

struct Sphere
{
	inline Sphere() = default;
	inline Sphere(const Position3 & c, float r) : center(c), radius(r) {}
	inline ~Sphere() = default;

	Position3 center;
	float radius;
};

inline bool intersects(const Ray & ray, const Sphere & sphere, float minDistance);

inline bool findIntersection(const Ray & ray, const Sphere & sphere, float minDistance, RayIntersection & intersection);

#include "sphere.hpp"

#endif
