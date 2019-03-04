#include "trianglemesh.h"
#include "triangle.h"
#include "ray.h"
#include "barycentric.h"

bool intersects(const Ray & ray, const TriangleMesh & mesh, float minDistance)
{
    return intersectsTrianglesIndexed(ray, &mesh.vertices[0], &mesh.indices[0], mesh.indices.size(), minDistance);
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

    const auto num_tri = indices.size() / 3;

    for(uint32_t tri_index = 0; tri_index < num_tri; ++tri_index) {
        uint32_t i0 = indices[3 * tri_index];
        uint32_t i1 = indices[3 * tri_index + 1];
        uint32_t i2 = indices[3 * tri_index + 2];
        if(intersectsTriangle(ray, vertices[i0], vertices[i1], vertices[i2],
                              minDistance, &t)
           && t < best_t) {
            best_tri = tri_index;
            best_t = t;
            hit = true;
        }
    }

    if(hit) {
        uint32_t i0 = indices[3 * best_tri];
        uint32_t i1 = indices[3 * best_tri + 1];
        uint32_t i2 = indices[3 * best_tri + 2];

        intersection.distance = best_t;
        intersection.position = ray.origin + ray.direction * best_t;

        auto bary = barycentricForPointInTriangle(intersection.position,
                                                  vertices[i0], vertices[i1], vertices[i2]);

        intersection.normal = interpolate(normals[i0], normals[i1], normals[i2],
                                          bary);
        // TODO: Texture coordinates
    }

    return hit;
}

