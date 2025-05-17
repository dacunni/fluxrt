#include "barycentric.h"
#include "coordinate.h"

inline bool intersects(const Ray & ray, const Triangle & triangle, float minDistance, float maxDistance)
{
    return intersectsTriangle(ray,
                              triangle.vertices[0],
                              triangle.vertices[1],
                              triangle.vertices[2],
                              minDistance, maxDistance);
}

inline bool findIntersection(const Ray & ray, const Triangle & triangle, float minDistance, RayIntersection & intersection)
{
    float t = FLT_MAX;
    const auto & v0 = triangle.vertices[0];
    const auto & v1 = triangle.vertices[1];
    const auto & v2 = triangle.vertices[2];
    if(!intersectsTriangle(ray, v0, v1, v2,
                           minDistance, std::numeric_limits<float>::max(),
                           &t)) {
        return false;
    }

    intersection.distance = t;
    intersection.position = ray.origin + ray.direction * t;
    //auto bary = barycentricForPointInTriangle(intersection.position, v0, v1, v2);
    intersection.normal = cross(v2 - v0, v1 - v0);
    // generate tangent / bitangent
    coordinate::coordinateSystem(intersection.normal, intersection.tangent, intersection.bitangent);
    return true;
}

inline bool intersectsTriangle(const Ray & ray,
                               const vec3 & v0, const vec3 & v1, const vec3 & v2,
                               float minDistance, float maxDistance,
                               float * distance)
{
    const float epsilon = 1.0e-6;

    // Compute edge vectors
    auto e1 = v1 - v0;
    auto e2 = v2 - v0;

    // Compute determinant
    auto P = cross(ray.direction, e2);
    auto det = dot(e1, P);

    // If determinant zero, the ray does not intersect the plane of the triangle
    // Note, we're not culling backfaces.
    if(std::fabs(det) < epsilon)
        return false;   // no intersection
    float inv_det = 1.0f / det;

    // Calculate vector from V0 to ray origin
    auto T = ray.origin - v0;

    // Calculate u coordinate and test whether the intersection lies within the valid range of u
    float u = inv_det * dot(T, P);
    if(u < 0.0f || u > 1.0f)
        return false;   // intersection out of valid u range

    // Calculate v coordinate and test whether the intersection lies within the valid range of v
    auto Q = cross(T, e1);
    float v = inv_det * dot(ray.direction, Q);
    if(v < 0.0f || u + v > 1.0f)
        return false;   // intersection out of u/v range

    float t = inv_det * dot(e2, Q);
    if(distance)
        *distance = t;
    return t >= minDistance && t <= maxDistance;
}

