#include <iostream>
#include <string>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "trianglemesh.h"
#include "triangle.h"
#include "ray.h"
#include "barycentric.h"
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
    float best_t = FLT_MAX, t = FLT_MAX;
    uint32_t best_tri = 0;
    bool hit = false;

    auto & vertices = mesh.vertices;
    auto & normals = mesh.normals;
    auto & indices = mesh.indices;

    const auto num_tri = indices.vertex.size() / 3;

    for(uint32_t tri_index = 0; tri_index < num_tri; ++tri_index) {
        uint32_t vi0 = indices.vertex[3 * tri_index + 0];
        uint32_t vi1 = indices.vertex[3 * tri_index + 1];
        uint32_t vi2 = indices.vertex[3 * tri_index + 2];
        if(intersectsTriangle(ray, vertices[vi0], vertices[vi1], vertices[vi2],
                              minDistance, &t)
           && t < best_t) {
            best_tri = tri_index;
            best_t = t;
            hit = true;
        }
    }

    if(hit) {
        auto vi0 = indices.vertex[3 * best_tri + 0];
        auto vi1 = indices.vertex[3 * best_tri + 1];
        auto vi2 = indices.vertex[3 * best_tri + 2];
        auto ni0 = indices.normal[3 * best_tri + 0];
        auto ni1 = indices.normal[3 * best_tri + 1];
        auto ni2 = indices.normal[3 * best_tri + 2];

        intersection.distance = best_t;
        intersection.position = ray.origin + ray.direction * best_t;

        auto bary = barycentricForPointInTriangle(intersection.position,
                                                  vertices[vi0], vertices[vi1], vertices[vi2]);

        intersection.normal = interpolate(normals[ni0], normals[ni1], normals[ni2],
                                          bary);
        // TODO: Texture coordinates
    }

    return hit;
}

void TriangleMesh::printMeta() const
{
    printf("TriangleMesh vertices %lu normals %lu indices v %lu n %lu\n",
           vertices.size(), normals.size(), indices.vertex.size(),
           indices.normal.size());
}


