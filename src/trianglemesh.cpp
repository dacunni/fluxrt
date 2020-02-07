#include <iostream>
#include <string>
#include <limits>

#include "trianglemesh.h"
#include "triangle.h"
#include "ray.h"
#include "barycentric.h"
#include "coordinate.h"
#include "slab.h"
#include "filesystem.h"

const uint32_t TriangleMesh::NoTexCoord = std::numeric_limits<uint32_t>::max();

bool loadTriangleMesh(TriangleMesh & mesh,
                      MaterialArray & materials,
                      TextureCache & textureCache,
                      const std::string & pathToFile)
{
    std::string path, filename;
    std::tie(path, filename) = filesystem::splitFileDirectory(pathToFile);

    if(filesystem::hasExtension(filename, ".obj")) {
        return loadTriangleMeshFromOBJ(mesh, materials, textureCache, path, filename);
    }
    else if(filesystem::hasExtension(filename, ".stl")) {
        return loadTriangleMeshFromSTL(mesh, materials, textureCache, path, filename);
    }

    std::cerr << "Unrecognized mesh type " << filename << '\n';
    return false;
}

bool intersects(const Ray & ray, const TriangleMesh & mesh, float minDistance, float maxDistance)
{
    return intersectsTrianglesIndexed(ray, &mesh.vertices[0], &mesh.indices.vertex[0], mesh.indices.vertex.size(),
                                      minDistance, maxDistance);
}

bool findIntersection(const Ray & ray, const TriangleMesh & mesh,
                      float minDistance, RayIntersection & intersection)
{
    float bestDistance = FLT_MAX, t = FLT_MAX;
    uint32_t bestTriangle = 0;
    bool hit = false;

    auto vertex = [&mesh](uint32_t tri, uint32_t index) { return mesh.triangleVertex(tri, index); };
    const auto numTriangles = mesh.numTriangles();

    for(uint32_t tri = 0; tri < numTriangles; ++tri) {
        if(intersectsTriangle(ray, vertex(tri, 0), vertex(tri, 1), vertex(tri, 2),
                              minDistance, std::numeric_limits<float>::max(), &t)
           && t < bestDistance) {
            bestTriangle = tri;
            bestDistance = t;
            hit = true;
        }
    }

    if(!hit)
        return false;

    fillTriangleMeshIntersection(ray, mesh, bestTriangle, bestDistance, intersection);

    return true;
}

void fillTriangleMeshIntersection(const Ray & ray, const TriangleMesh & mesh,
                                  uint32_t tri, float t, RayIntersection & intersection)
{
    intersection.distance = t;
    intersection.position = ray.origin + ray.direction * t;

    // Use material override if present, otherwise use mesh material
    if(mesh.material != NoMaterial) {
        intersection.material = mesh.material;
    }
    else {
        intersection.material = mesh.faces.material[tri];
    }

    auto bary = barycentricForPointInTriangle(intersection.position,
                                              mesh.triangleVertex(tri, 0),
                                              mesh.triangleVertex(tri, 1),
                                              mesh.triangleVertex(tri, 2));

    if(mesh.hasNormals()) {
#if 1
        // TODO: Remove this once we condition input normals to be
        //       pointing consistently in the same direction.
        Direction3 n0 = mesh.triangleNormal(tri, 0);
        Direction3 n1 = mesh.triangleNormal(tri, 1);
        Direction3 n2 = mesh.triangleNormal(tri, 2);
        if(dot(n0, ray.direction) > 0.0f) { n0.negate(); }
        if(dot(n1, ray.direction) > 0.0f) { n1.negate(); }
        if(dot(n2, ray.direction) > 0.0f) { n2.negate(); }
        intersection.normal = interpolate(n0, n1, n2, bary);
#else
        intersection.normal = interpolate(mesh.triangleNormal(tri, 0),
                                          mesh.triangleNormal(tri, 1),
                                          mesh.triangleNormal(tri, 2), bary);
#endif
    }
    else {
        // Generate normals from triangle vertices
        // TODO: This will give flat shading. It would be nice to generate
        //       per vertex normals on mesh load so we can just interpolate
        //       here as usual.
        auto v0 = mesh.triangleVertex(tri, 0);
        auto v1 = mesh.triangleVertex(tri, 1);
        auto v2 = mesh.triangleVertex(tri, 2);
        intersection.normal = cross(v2 - v0, v1 - v0).normalized();
        if(dot(ray.direction, intersection.normal) > 0.0f) {
            intersection.normal.negate();
        }
    }

    // Interpolate texture coordinates, if available
    auto tci0 = mesh.indices.texcoord[3 * tri + 0];
    auto tci1 = mesh.indices.texcoord[3 * tri + 1];
    auto tci2 = mesh.indices.texcoord[3 * tri + 2];

    if(tci0 != TriangleMesh::NoTexCoord && tci1 != TriangleMesh::NoTexCoord && tci2 != TriangleMesh::NoTexCoord) {
        intersection.texcoord = interpolate(mesh.texcoords[tci0],
                                            mesh.texcoords[tci1],
                                            mesh.texcoords[tci2], bary);
        intersection.hasTexCoord = true;
    }
    else {
        intersection.texcoord = { 0.0f, 0.0f };
        intersection.hasTexCoord = false;
    }

    // generate tangent / bitangent
    coordinate::coordinateSystem(intersection.normal, intersection.tangent, intersection.bitangent);
}

size_t TriangleMesh::numTriangles() const
{
    return indices.vertex.size() / 3;
}

bool TriangleMesh::hasNormals() const
{
    return normals.size() > 0;
}

const Position3 & TriangleMesh::triangleVertex(uint32_t tri, uint32_t index) const
{
    return vertices[indices.vertex[3 * tri + index]];
}

const Direction3 & TriangleMesh::triangleNormal(uint32_t tri, uint32_t index) const
{
    return normals[indices.normal[3 * tri + index]];
}

const TextureCoordinate & TriangleMesh::triangleTextureCoordinate(uint32_t tri, uint32_t index) const
{
    return texcoords[indices.texcoord[3 * tri + index]];
}

void TriangleMesh::printMeta() const
{
    printf("TriangleMesh vertices %lu normals %lu indices v %lu n %lu\n",
           vertices.size(), normals.size(), indices.vertex.size(), indices.normal.size());
}

void TriangleMesh::scaleToFit(const Slab & bounds)
{
    Slab old = boundingBox(vertices);

    auto s = relativeScale(old, bounds);
    auto mine = s.minElement();
    auto maxe = s.maxElement();

    float scaleFactor = 1.0f;

    if(maxe < 1.0f) {
        // scale down
        scaleFactor = maxe;
    }
    else {
        // scale up
        scaleFactor = mine;
    }

    auto oldCenter = old.midpoint();
    auto newCenter = bounds.midpoint();

    // Scale to fit and center in the new bounding volume
    for(auto & v : vertices) {
        v.x = (v.x - oldCenter.x) * scaleFactor + newCenter.x;
        v.y = (v.y - oldCenter.y) * scaleFactor + newCenter.y;
        v.z = (v.z - oldCenter.z) * scaleFactor + newCenter.z;
    }

    // NOTE: We don't need to adjust normals be cause we are scaling uniformly in all directions
}

