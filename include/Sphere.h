#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "traceable.h"
#include "Ray.h"
#include "material.h"

struct Sphere : public Traceable
{
	inline Sphere() = default;
	inline Sphere(const Position3 & c, float r) : center(c), radius(r) {}
	inline ~Sphere() = default;

    // Ray intersection implementation
    inline virtual bool intersects(const Ray & ray, float minDistance, float maxDistance) const override;
    inline virtual bool findIntersection(const Ray & ray, float minDistance, RayIntersection & intersection) const override;

    inline bool intersectHelper(const Ray & ray, float & dist1, float & dist2) const;

	Position3 center = Position3{ 0.0f, 0.0f, 0.0f };
	float radius = 1.0f;
    MaterialID material = NoMaterial;
};

#include "Sphere.hpp"
#endif
