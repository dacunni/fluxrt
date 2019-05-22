#include <iostream>
#include <string>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "trianglemesh.h"
#include "triangle.h"
#include "ray.h"
#include "barycentric.h"
#include "coordinate.h"
#include "slab.h"

static bool hasExtension(const std::string & filename, const std::string & ending) {
    if (ending.size() > filename.size()) {
        return false;
    }
    return std::equal(ending.rbegin(), ending.rend(), filename.rbegin(),
                      [](const char a, const char b) {
                          return tolower(a) == tolower(b);
                      });
}

// Wavefront OBJ format
bool loadTriangleMeshFromOBJ(TriangleMesh & mesh,
                             const std::string & path, const std::string & filename)
{
    std::string warn, err;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                                (path + '/' + filename).c_str(), path.c_str());
    if(!warn.empty()) {
        std::cout << "WARN: " << warn << std::endl;
    }
    if(!err.empty()) {
        std::cerr << err << std::endl;
    }

    if(!ret) {
        std::cerr << "Failed to load " << filename << std::endl;
        return false;
    }

    printf("OBJ Mesh:\n");
    printf("    vertices  = %d\n", (int)(attrib.vertices.size()) / 3);
    printf("    normals   = %d\n", (int)(attrib.normals.size()) / 3);
    printf("    texcoords = %d\n", (int)(attrib.texcoords.size()) / 2);
    printf("    materials = %d\n", (int)materials.size());
    printf("    shapes    = %d\n", (int)shapes.size());

    materials.push_back(tinyobj::material_t()); // default material

    for(int vi = 0; vi < attrib.vertices.size() / 3; ++vi) {
        auto coord = &attrib.vertices[vi * 3];
        Position3 pos = Position3(coord[0], coord[1], coord[2]);
        mesh.vertices.push_back(pos);
    }

    for(int ni = 0; ni < attrib.normals.size() / 3; ++ni) {
        auto coord = &attrib.normals[ni * 3];
        Direction3 dir = Direction3(coord[0], coord[1], coord[2]);
        mesh.normals.push_back(dir);
    }

    // shapes
    for(size_t si = 0; si < shapes.size(); ++si) {
        auto num_faces = shapes[si].mesh.indices.size() / 3;
        // faces
        for(size_t fi = 0; fi < num_faces; ++fi) {
            auto indices = &shapes[si].mesh.indices[3 * fi];
            // vertex indices
            for (int vi = 0; vi < 3; ++vi) {
                mesh.indices.vertex.push_back(indices[vi].vertex_index);
                mesh.indices.normal.push_back(indices[vi].normal_index);
            }
        }
    }

    // TODO: Texture coordinates
    // TODO: Materials
    // TODO: Handle missing normals

    return true;
}

bool loadTriangleMesh(TriangleMesh & mesh,
                      const std::string & path, const std::string & filename)
{
    if(hasExtension(filename, ".obj")) {
        return loadTriangleMeshFromOBJ(mesh, path, filename);
    }

    std::cerr << "Unrecognized mesh type " << filename << '\n';
    return false;
}

bool intersects(const Ray & ray, const TriangleMesh & mesh, float minDistance)
{
    return intersectsTrianglesIndexed(ray, &mesh.vertices[0], &mesh.indices.vertex[0], mesh.indices.vertex.size(), minDistance);
}

bool findIntersection(const Ray & ray, const TriangleMesh & mesh,
                      float minDistance, RayIntersection & intersection)
{
    float bestDistance = FLT_MAX, t = FLT_MAX;
    uint32_t bestTriangle = 0;
    bool hit = false;

    auto vertex = [&mesh](uint32_t tri, uint32_t index) { return mesh.triangleVertex(tri, index); };
    auto normal = [&mesh](uint32_t tri, uint32_t index) { return mesh.triangleNormal(tri, index); };
    const auto numTriangles = mesh.numTriangles();

    for(uint32_t tri = 0; tri < numTriangles; ++tri) {
        if(intersectsTriangle(ray, vertex(tri, 0), vertex(tri, 1), vertex(tri, 2), minDistance, &t)
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
    auto vertex = [&mesh](uint32_t tri, uint32_t index) { return mesh.triangleVertex(tri, index); };
    auto normal = [&mesh](uint32_t tri, uint32_t index) { return mesh.triangleNormal(tri, index); };

    intersection.distance = t;
    intersection.position = ray.origin + ray.direction * t;

    auto bary = barycentricForPointInTriangle(intersection.position,
                                              vertex(tri, 0), vertex(tri, 1), vertex(tri, 2));

    assert(mesh.hasNormals() && "TODO: Implement normal generation");
    intersection.normal = interpolate(normal(tri, 0), normal(tri, 1), normal(tri, 2), bary);

    // TODO: Texture coordinates

    // TODO: Generate tangent/bitangent from texture coordinates if available

    // generate tangent / bitangent
    coordinate::coordinateSystem(intersection.normal, intersection.tangent, intersection.bitangent);
}

size_t TriangleMesh::numTriangles() const
{
    return indices.vertex.size() / 3;
}

bool TriangleMesh::hasNormals() const
{
    return indices.normal.size() > 0;
}

const Position3 & TriangleMesh::triangleVertex(uint32_t tri, uint32_t index) const
{
    return vertices[indices.vertex[3 * tri + index]];
}

const Direction3 & TriangleMesh::triangleNormal(uint32_t tri, uint32_t index) const
{
    return normals[indices.normal[3 * tri + index]];
}

void TriangleMesh::printMeta() const
{
    printf("TriangleMesh vertices %lu normals %lu indices v %lu n %lu\n",
           vertices.size(), normals.size(), indices.vertex.size(), indices.normal.size());
}


