#include <cassert>
#include "triangle.h"

bool intersectsTriangles(const Ray & ray, const vec3 vertices[], size_t nvertices, float minDistance)
{
    assert(nvertices % 3 == 0);

    for(size_t i = 0; i < nvertices; i += 3) {
        if(intersectsTriangle(ray, vertices[i], vertices[i+1], vertices[i+2], minDistance)) {
            return true;
        }
    }

    return false;
}

bool intersectsTriangleStrip(const Ray & ray, const vec3 vertices[], size_t nvertices, float minDistance)
{
    assert(nvertices >= 3);

    for(size_t i = 0; i < nvertices - 2; ++i ) {
        // alternate ordering of second and third indices so even triangles
        // winding order is corrected to match odd triangles
        if(i % 2) {
            if(intersectsTriangle(ray, vertices[i], vertices[i+2], vertices[i+1], minDistance)) {
                return true;
            }
        }
        else {
            if(intersectsTriangle(ray, vertices[i], vertices[i+1], vertices[i+2], minDistance)) {
                return true;
            }
        }
    }

    return false;
}

