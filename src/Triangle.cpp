#include <cassert>
#include "Triangle.h"

bool intersectsTriangles(const Ray & ray, const vec3 vertices[], size_t nvertices, float minDistance, float maxDistance)
{
    assert(nvertices % 3 == 0);

    for(size_t i = 0; i < nvertices; i += 3) {
        if(intersectsTriangle(ray, vertices[i], vertices[i+1], vertices[i+2], minDistance, maxDistance)) {
            return true;
        }
    }

    return false;
}

template<typename INDEX_TYPE>
static inline bool intersectsTrianglesIndexed_t(const Ray & ray,
                                  const vec3 vertices[],
                                  const INDEX_TYPE indices[], size_t nindices,
                                  float minDistance, float maxDistance)
{
    assert(nindices % 3 == 0);

    for(size_t i = 0; i < nindices; i += 3) {
        auto i0 = indices[i];
        auto i1 = indices[i+1];
        auto i2 = indices[i+2];
        if(intersectsTriangle(ray, vertices[i0], vertices[i1], vertices[i2], minDistance, maxDistance)) {
            return true;
        }
    }

    return false;
}

bool intersectsTrianglesIndexed(const Ray & ray,
                                const vec3 vertices[],
                                const uint32_t indices[], size_t nindices,
                                float minDistance, float maxDistance)
{
    return intersectsTrianglesIndexed_t(ray, vertices, indices, nindices, minDistance, maxDistance);
}

bool intersectsTrianglesIndexed(const Ray & ray,
                                const vec3 vertices[],
                                const uint16_t indices[], size_t nindices,
                                float minDistance, float maxDistance)
{
    return intersectsTrianglesIndexed_t(ray, vertices, indices, nindices, minDistance, maxDistance);
}

bool intersectsTriangleStrip(const Ray & ray,
                             const vec3 vertices[], size_t nvertices,
                             float minDistance, float maxDistance)
{
    assert(nvertices >= 3);

    for(size_t i = 0; i < nvertices - 2; ++i ) {
        // alternate ordering of second and third indices so even triangles
        // winding order is corrected to match odd triangles
        if(i % 2) {
            if(intersectsTriangle(ray, vertices[i], vertices[i+2], vertices[i+1], minDistance, maxDistance)) {
                return true;
            }
        }
        else {
            if(intersectsTriangle(ray, vertices[i], vertices[i+1], vertices[i+2], minDistance, maxDistance)) {
                return true;
            }
        }
    }

    return false;
}

bool intersectsTriangles(const Ray rays[], size_t nrays,
                         bool hits[],
                         const vec3 vertices[], size_t nvertices,
                         float minDistance, float maxDistance)
{
    assert(nvertices % 3 == 0);

    for(size_t ri = 0; ri < nrays; ++ri) { hits[ri] = false; }
    bool hit_any = false;

    for(size_t i = 0; i < nvertices; i += 3) {
        for(size_t ri = 0; ri < nrays; ++ri) {
            if(hits[ri]) {
                continue;
            }
            if(intersectsTriangle(rays[ri], vertices[i], vertices[i+1], vertices[i+2], minDistance, maxDistance)) {
                hits[ri] = true;
                hit_any = true;
            }
        }
    }

    return hit_any;
}

