#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include <cmath>
#include "ray.h"

struct Triangle
{
	inline Triangle() = default;
	inline ~Triangle() = default;

    Position3 vertices[3];
};


// Ray intersection
inline bool intersects(const Ray & ray, const Triangle & triangle, float minDistance);
inline bool findIntersection(const Ray & ray, const Triangle & triangle, float minDistance, RayIntersection & intersection);

inline bool intersectsTriangle(const Ray & ray,
                               const vec3 & v0, const vec3 & v1, const vec3 & v2,
                               float minDistance,
                               float * distance = nullptr);

// ray / triangle array
bool intersectsTriangles(const Ray & ray,
                         const vec3 vertices[], size_t nvertices,
                         float minDistance);

// ray / triangle array (indexed)
bool intersectsTrianglesIndexed(const Ray & ray,
                                const vec3 vertices[],
                                const uint32_t indices[], size_t nindices,
                                float minDistance);
bool intersectsTrianglesIndexed(const Ray & ray,
                                const vec3 vertices[],
                                const uint16_t indices[], size_t nindices,
                                float minDistance);

// ray / triangle strip
bool intersectsTriangleStrip(const Ray & ray,
                             const vec3 vertices[], size_t nvertices,
                             float minDistance);

// ray bundle / triangle array
bool intersectsTriangles(const Ray rays[], size_t nrays,
                         bool hits[],
                         const vec3 vertices[], size_t nvertices,
                         float minDistance);

#include "triangle.hpp"
#endif