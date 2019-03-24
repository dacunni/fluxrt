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

    size_t numTriangles() const;
    bool hasNormals() const;

    const Position3 & triangleVertex(uint32_t tri, uint32_t index) const;
    const Direction3 & triangleNormal(uint32_t tri, uint32_t index) const;

    void printMeta() const;

    std::vector<Position3> vertices;
    std::vector<Direction3> normals;
    struct {
        std::vector<uint32_t> vertex;
        std::vector<uint32_t> normal;
    } indices;
};

bool loadTriangleMesh(TriangleMesh & mesh,
                      const std::string & path, const std::string & filename);

// Ray intersection
bool intersects(const Ray & ray, const TriangleMesh & mesh, float minDistance);
bool findIntersection(const Ray & ray, const TriangleMesh & mesh, float minDistance, RayIntersection & intersection);

#endif
