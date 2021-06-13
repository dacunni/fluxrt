#include <iostream>
#include <string>
#include <limits>

#include "TriangleMesh.h"
#include "Triangle.h"
#include "Ray.h"
#include "barycentric.h"
#include "coordinate.h"
#include "filesystem.h"

const uint32_t TriangleMeshData::NoTexCoord = std::numeric_limits<uint32_t>::max();

bool loadTriangleMesh(TriangleMesh & mesh,
                      MaterialArray & materials,
                      TriangleMeshDataCache & meshDataCache,
                      TextureCache & textureCache,
                      const std::string & pathToFile)
{
    // Check the mesh data cache
    auto fm = meshDataCache.fileToMeshData.find(pathToFile);

    if(fm != meshDataCache.fileToMeshData.end()) {
        // Mesh data is in the cache
        mesh.meshData = fm->second;
        printf("Mesh data cache hit\n");
        return true;
    }

    // Cache miss, load file and update the cache

    std::string path, filename;
    std::tie(path, filename) = filesystem::splitFileDirectory(pathToFile);

    bool success = false;

    if(filesystem::hasExtension(filename, ".obj")) {
        success = loadTriangleMeshFromOBJ(mesh, materials, meshDataCache, textureCache, path, filename);
    }
    else if(filesystem::hasExtension(filename, ".stl")) {
        success = loadTriangleMeshFromSTL(mesh, materials, meshDataCache, textureCache, path, filename);
    }
    else {
        std::cerr << "Unrecognized mesh type " << filename << '\n';
        success = false;
    }

    if(success) {
        // Update the cache
        meshDataCache.fileToMeshData[pathToFile] = mesh.meshData;
    }

    return success;
}

bool TriangleMesh::intersects(const Ray & ray, float minDistance, float maxDistance) const
{
    return intersectsTrianglesIndexed(ray, &meshData->vertices[0], &meshData->indices.vertex[0], meshData->indices.vertex.size(),
                                      minDistance, maxDistance);
}

bool TriangleMesh::findIntersection(const Ray & ray, float minDistance, RayIntersection & intersection) const
{
    float bestDistance = FLT_MAX, t = FLT_MAX;
    uint32_t bestTriangle = 0;
    bool hit = false;

    auto vertex = [&](uint32_t tri, uint32_t index) { return triangleVertex(tri, index); };
    const auto numTriangles = this->numTriangles();

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

    fillTriangleMeshIntersection(ray, bestTriangle, bestDistance, intersection);

    return true;
}

Slab TriangleMesh::boundingBox()
{
    return meshData->bounds;
}

void TriangleMesh::fillTriangleMeshIntersection(const Ray & ray, uint32_t tri, float t, RayIntersection & intersection) const
{
    intersection.distance = t;
    intersection.position = ray.origin + ray.direction * t;

    // Use material override if present, otherwise use mesh material
    if(material != NoMaterial) {
        intersection.material = material;
    }
    else {
        intersection.material = meshData->faces.material[tri];
    }

    auto bary = barycentricForPointInTriangle(intersection.position,
                                              triangleVertex(tri, 0),
                                              triangleVertex(tri, 1),
                                              triangleVertex(tri, 2));

    if(hasNormals()) {
#if 1
        // TODO: Remove this once we condition input normals to be
        //       pointing consistently in the same direction.
        Direction3 n0 = triangleNormal(tri, 0);
        Direction3 n1 = triangleNormal(tri, 1);
        Direction3 n2 = triangleNormal(tri, 2);
        if(dot(n0, ray.direction) > 0.0f) { n0.negate(); }
        if(dot(n1, ray.direction) > 0.0f) { n1.negate(); }
        if(dot(n2, ray.direction) > 0.0f) { n2.negate(); }
        intersection.normal = interpolate(n0, n1, n2, bary);
#else
        intersection.normal = interpolate(triangleNormal(tri, 0),
                                          triangleNormal(tri, 1),
                                          triangleNormal(tri, 2), bary);
#endif
    }
    else {
        // Generate normals from triangle vertices
        // TODO: This will give flat shading. It would be nice to generate
        //       per vertex normals on mesh load so we can just interpolate
        //       here as usual.
        auto v0 = triangleVertex(tri, 0);
        auto v1 = triangleVertex(tri, 1);
        auto v2 = triangleVertex(tri, 2);
        intersection.normal = cross(v2 - v0, v1 - v0).normalized();
        if(dot(ray.direction, intersection.normal) > 0.0f) {
            intersection.normal.negate();
        }
    }

    // Interpolate texture coordinates, if available, and generate tangent/bitangent vectors

    auto tci0 = meshData->indices.texcoord[3 * tri + 0];
    auto tci1 = meshData->indices.texcoord[3 * tri + 1];
    auto tci2 = meshData->indices.texcoord[3 * tri + 2];

    if(tci0 != TriangleMeshData::NoTexCoord &&
       tci1 != TriangleMeshData::NoTexCoord &&
       tci2 != TriangleMeshData::NoTexCoord) {
        // FIXME: This branch produces faceted artifacts in the tangent
        //        and bitangent with the mori model.
        TextureCoordinate tc0 = meshData->texcoords[tci0];
        TextureCoordinate tc1 = meshData->texcoords[tci1];
        TextureCoordinate tc2 = meshData->texcoords[tci2];

        intersection.texcoord = interpolate(tc0, tc1, tc2, bary);
        intersection.hasTexCoord = true;

        // Generate tangent / bitangent
        //   Reference: https://www.cs.utexas.edu/~fussell/courses/cs384g-spring2016/lectures/normal_mapping_tangent.pdf
        Position3 P0 = triangleVertex(tri, 0);
        Position3 P1 = triangleVertex(tri, 1);
        Position3 P2 = triangleVertex(tri, 2);

        Direction3 V1 = P1 - P0;
        Direction3 V2 = P2 - P0;

        float du1 = tc1.u - tc0.u;
        float dv1 = tc1.v - tc0.v;
        float du2 = tc2.u - tc0.u;
        float dv2 = tc2.v - tc0.v;

        float sf = du1 * dv2 - du2 * dv1;

        if(sf == 0.0f)
            sf = 1.0f;

        intersection.tangent   = ( dv2 * V1 - dv1 * V2) / sf;
        intersection.bitangent = (-du2 * V1 + du1 * V2) / sf;

        intersection.tangent.normalize();
        intersection.bitangent.normalize();

        // Make sure we have a right handed coordinate system
        if(dot(cross(intersection.normal, intersection.tangent),
               intersection.bitangent) < 0.0f) {
            intersection.bitangent.negate();
        }
    }
    else {
        intersection.texcoord = { 0.0f, 0.0f };
        intersection.hasTexCoord = false;

        // Generate tangent / bitangent
        coordinate::coordinateSystem(intersection.normal, intersection.tangent, intersection.bitangent);
    }
}

size_t TriangleMesh::numTriangles() const
{
    return meshData->indices.vertex.size() / 3;
}

bool TriangleMesh::hasNormals() const
{
    return meshData->normals.size() > 0;
}

const Position3 & TriangleMesh::triangleVertex(uint32_t tri, uint32_t index) const
{
    return meshData->vertices[meshData->indices.vertex[3 * tri + index]];
}

const Direction3 & TriangleMesh::triangleNormal(uint32_t tri, uint32_t index) const
{
    return meshData->normals[meshData->indices.normal[3 * tri + index]];
}

const TextureCoordinate & TriangleMesh::triangleTextureCoordinate(uint32_t tri, uint32_t index) const
{
    return meshData->texcoords[meshData->indices.texcoord[3 * tri + index]];
}

void TriangleMesh::printMeta() const
{
    printf("TriangleMesh vertices %lu normals %lu indices v %lu n %lu\n",
           meshData->vertices.size(), meshData->normals.size(),
           meshData->indices.vertex.size(), meshData->indices.normal.size());
}

void TriangleMesh::scaleToFit(const Slab & bounds)
{
#if 1
    printf("WARNING TriangleMesh::scaleToFit needs to be updated to create "
           "a scaling matrix so mesh data can be shared\n");
    return;
#else
    Slab old = ::boundingBox(vertices);

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

#endif
    // NOTE: We don't need to adjust normals be cause we are scaling uniformly in all directions
}

