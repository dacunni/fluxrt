#include <cmath>
#include <sstream>
#include "coordinate.h"
#include "slab.h"

inline bool intersectPlane(const Ray & ray,
                           const Position3 & pointOnPlane,
                           const Direction3 & N,
                           float & t)
{
    float denom = dot(N, ray.direction);

    if (std::abs(denom) > 1.0e-6f) {
        vec3 originToPOP = pointOnPlane - ray.origin;
        t = dot(originToPOP, N) / denom;
        return t >= 0;
    }

    return false;
}

inline bool DiskLight::intersects(const Ray & ray, float minDistance, float maxDistance) const
{
    RayIntersection intersection;

    if(findIntersection(ray, minDistance, intersection) &&
       intersection.distance <= maxDistance) {
        return true;
    }

    return false;
}

inline bool DiskLight::findIntersection(const Ray & ray, float minDistance, RayIntersection & intersection) const
{
    float t = 0;

    if(intersectPlane(ray, position, direction, t)) {
        if(t < minDistance) {
            return false;
        }
        Position3 p = ray.origin + ray.direction * t;
        vec3 v = p - position;
        float d2 = dot(v, v);
        if(d2 <= radius * radius) {
            intersection.distance = t;
            // compute intersection position
            intersection.position = add(ray.origin, scale(ray.direction, intersection.distance));
            // compute surface normal
            intersection.normal = direction;
            // generate tangent / bitangent
            coordinate::coordinateSystem(intersection.normal, intersection.tangent, intersection.bitangent);
            intersection.material = material;
            return true;
        }
     }

     return false;
}

inline Slab DiskLight::boundingBox()
{
    auto extent = radius * Direction3(
        cross(direction, Direction3(1.0f, 0.0f, 0.0f)).magnitude(),
        cross(direction, Direction3(0.0f, 1.0f, 0.0f)).magnitude(),
        cross(direction, Direction3(0.0f, 0.0f, 1.0f)).magnitude()
    );
        
    return Slab(position - extent, position + extent);
}

