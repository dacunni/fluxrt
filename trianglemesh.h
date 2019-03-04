#ifndef __TRIANGLE_MESH_H__
#define __TRIANGLE_MESH_H__

#include <vector>
#include "vectortypes.h"
struct Ray;
struct RayIntersection;

struct TriangleMesh
{
    inline TriangleMesh() = default;
    inline ~TriangleMesh() = default;

    std::vector<Position3> vertices;
    std::vector<Direction3> normals;
    std::vector<uint32_t> indices;
};

// Ray intersection
bool intersects(const Ray & ray, const TriangleMesh & mesh, float minDistance);
bool findIntersection(const Ray & ray, const TriangleMesh & mesh, float minDistance, RayIntersection & intersection);

#endif
