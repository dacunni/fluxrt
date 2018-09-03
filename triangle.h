#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include <cmath>
#include "ray.h"

struct Triangle
{
	inline Triangle() = default;
	inline ~Triangle() = default;

    vec3 vertices[3];
};

// TODO - all of this

// Ray intersection
//inline bool intersects(const Ray & ray, const Triangle & triangle, float minDistance);
//inline bool findIntersection(const Ray & ray, const Triangle & triangle, float minDistance, RayIntersection & intersection);

inline bool intersectsTriangle(const Ray & ray,
                               const vec3 & v0, const vec3 & v1, const vec3 & v2,
                               float minDistance);
bool intersectsTriangles(const Ray & ray,
                         const vec3 vertices[], size_t nvertices,
                         float minDistance);
bool intersectsTriangleStrip(const Ray & ray,
                             const vec3 vertices[], size_t nvertices,
                             float minDistance);

#include "triangle.hpp"
#endif
