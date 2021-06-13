#ifndef __TRIANGLE_MESH_H__
#define __TRIANGLE_MESH_H__

#include <vector>
#include "vectortypes.h"
#include "material.h"
#include "traceable.h"
#include "slab.h"

struct Ray;
struct RayIntersection;

struct TriangleMeshData
{
    std::vector<Position3> vertices;
    std::vector<Direction3> normals;
    std::vector<TextureCoordinate> texcoords;

    // Special texcoord index indicating no texture coordinates exist for the vertex
    static const uint32_t NoTexCoord;

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

    Slab bounds;
};

struct TriangleMesh : public Traceable
{
    inline TriangleMesh() = default;
    inline ~TriangleMesh() = default;

    // Ray intersection implementation
    virtual bool intersects(const Ray & ray, float minDistance, float maxDistance) const override;
    virtual bool findIntersection(const Ray & ray, float minDistance, RayIntersection & intersection) const override;

    // Bounding volume
    Slab boundingBox() override;

    void fillTriangleMeshIntersection(const Ray & ray, uint32_t tri, float t, RayIntersection & intersection) const;

    size_t numTriangles() const;
    bool hasNormals() const;

    const Position3 & triangleVertex(uint32_t tri, uint32_t index) const;
    const Direction3 & triangleNormal(uint32_t tri, uint32_t index) const;
    const TextureCoordinate & triangleTextureCoordinate(uint32_t tri, uint32_t index) const;

    void printMeta() const;

    void scaleToFit(const Slab & bounds);

    std::shared_ptr<TriangleMeshData> meshData;

    // Global material override
    MaterialID material = NoMaterial;
};

bool loadTriangleMesh(TriangleMesh & mesh,
                      MaterialArray & materials,
                      TextureCache & textureCache,
                      const std::string & pathToFile);
bool loadTriangleMeshFromOBJ(TriangleMesh & mesh,
                             MaterialArray & materials,
                             TextureCache & textureCache,
                             const std::string & path, const std::string & filename);
bool loadTriangleMeshFromSTL(TriangleMesh & mesh,
                             MaterialArray & materials,
                             TextureCache & textureCache,
                             const std::string & path, const std::string & filename);

#endif
