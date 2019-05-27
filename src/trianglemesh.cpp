#include <iostream>
#include <string>
#include <limits>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "trianglemesh.h"
#include "triangle.h"
#include "ray.h"
#include "barycentric.h"
#include "coordinate.h"
#include "slab.h"

// Special texcoord index indicating no texture coordinates exist for the vertex
const uint32_t NoTexCoord = std::numeric_limits<uint32_t>::max();

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

    printf("OBJ Mesh:");
    printf(" vertices: %d", (int)(attrib.vertices.size()) / 3);
    printf(" normals: %d", (int)(attrib.normals.size()) / 3);
    printf(" texcoords: %d", (int)(attrib.texcoords.size()) / 2);
    printf(" materials: %d", (int)materials.size());
    printf(" shapes: %d\n", (int)shapes.size());

    materials.push_back(tinyobj::material_t()); // default material

    for(int vi = 0; vi < attrib.vertices.size() / 3; ++vi) {
        auto coord = &attrib.vertices[vi * 3];
        auto pos = Position3(coord[0], coord[1], coord[2]);
        mesh.vertices.push_back(pos);
    }

    for(int ni = 0; ni < attrib.normals.size() / 3; ++ni) {
        auto coord = &attrib.normals[ni * 3];
        auto dir = Direction3(coord[0], coord[1], coord[2]);
        mesh.normals.push_back(dir);
    }

    for(int ti = 0; ti < attrib.texcoords.size() / 2; ++ti) {
        auto coord = &attrib.texcoords[ti * 2];
        // TODO: The tinyobj example flips v. Need to double check this is right.
        //auto tc = TextureCoordinate{coord[0], coord[1]};
        auto tc = TextureCoordinate{coord[0], 1.0f - coord[1]};
        mesh.texcoords.push_back(tc);
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
                if(indices[vi].texcoord_index < 0) {
                    mesh.indices.texcoord.push_back(NoTexCoord);
                }
                else {
                    mesh.indices.texcoord.push_back(indices[vi].texcoord_index);
                }
            }
        }
    }

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
    intersection.distance = t;
    intersection.position = ray.origin + ray.direction * t;

    auto bary = barycentricForPointInTriangle(intersection.position,
                                              mesh.triangleVertex(tri, 0),
                                              mesh.triangleVertex(tri, 1),
                                              mesh.triangleVertex(tri, 2));

    assert(mesh.hasNormals() && "TODO: Implement normal generation");
    intersection.normal = interpolate(mesh.triangleNormal(tri, 0),
                                      mesh.triangleNormal(tri, 1),
                                      mesh.triangleNormal(tri, 2), bary);

    // Interpolate texture coordinates, if available
    auto tci0 = mesh.indices.texcoord[3 * tri + 0];
    auto tci1 = mesh.indices.texcoord[3 * tri + 1];
    auto tci2 = mesh.indices.texcoord[3 * tri + 2];

    if(tci0 != NoTexCoord && tci1 != NoTexCoord && tci2 != NoTexCoord) {
        intersection.texcoord = interpolate(mesh.texcoords[mesh.indices.texcoord[tci0]],
                                            mesh.texcoords[mesh.indices.texcoord[tci1]],
                                            mesh.texcoords[mesh.indices.texcoord[tci2]], bary);
        intersection.hasTexCoord = true;
    }
    else {
        intersection.texcoord = { 0.0f, 0.0f };
        intersection.hasTexCoord = false;
    }

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
        scaleFactor = mine;
    }
    else {
        // scale up
        scaleFactor = 1.0f / maxe;
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

