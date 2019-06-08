#ifndef __TRIANGLE_MESH_H__
#define __TRIANGLE_MESH_H__

#include <vector>
#include "vectortypes.h"
#include "material.h"

struct Ray;
struct RayIntersection;
struct Slab;

struct TriangleMesh
{
    inline TriangleMesh() = default;
    inline ~TriangleMesh() = default;

    size_t numTriangles() const;
    bool hasNormals() const;

    const Position3 & triangleVertex(uint32_t tri, uint32_t index) const;
    const Direction3 & triangleNormal(uint32_t tri, uint32_t index) const;
    const TextureCoordinate & triangleTextureCoordinate(uint32_t tri, uint32_t index) const;

    void printMeta() const;

    void scaleToFit(const Slab & bounds);

    std::vector<Position3> vertices;
    std::vector<Direction3> normals;
    std::vector<TextureCoordinate> texcoords;

    // Per-vertex properties
    struct {
        std::vector<uint32_t> vertex;
        std::vector<uint32_t> normal;
        std::vector<uint32_t> texcoord;
    } indices;

    // Per-face properties
    struct {
        std::vector<MaterialID> material;
    } faces;
};

bool loadTriangleMesh(TriangleMesh & mesh, std::vector<Material> & materials,
                      const std::string & path, const std::string & filename);

// Ray intersection
bool intersects(const Ray & ray, const TriangleMesh & mesh, float minDistance);
bool findIntersection(const Ray & ray, const TriangleMesh & mesh, float minDistance, RayIntersection & intersection);
void fillTriangleMeshIntersection(const Ray & ray, const TriangleMesh & mesh, uint32_t tri, float t, RayIntersection & intersection);

#endif
