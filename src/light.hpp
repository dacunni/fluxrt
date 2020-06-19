#include <cmath>
#include <sstream>
#include "coordinate.h"

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

inline bool intersects(const Ray & ray, const DiskLight & light, float minDistance, float maxDistance)
{
    RayIntersection intersection;
    return findIntersection(ray, light, minDistance, intersection);
}

inline bool findIntersection(const Ray & ray, const DiskLight & light, float minDistance, RayIntersection & intersection)
{
    float t = 0;

    if(intersectPlane(ray, light.position, light.direction, t)) {
        Position3 p = ray.origin + ray.direction * t;
        vec3 v = p - light.position;
        float d2 = dot(v, v);
        if(d2 <= light.radius * light.radius) {
            intersection.distance = t;
            // compute intersection position
            intersection.position = add(ray.origin, scale(ray.direction, intersection.distance));
            // compute surface normal
            intersection.normal = light.direction;
            // generate tangent / bitangent
            coordinate::coordinateSystem(intersection.normal, intersection.tangent, intersection.bitangent);
            // TODO - material?
            //intersection.material = light.material;
            return true;
        }
     }

     return false;
}

