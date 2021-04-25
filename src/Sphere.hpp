#include <cmath>
#include <sstream>
#include "coordinate.h"
#include "slab.h"

inline bool Sphere::intersectHelper(const Ray & ray, float & dist1, float & dist2) const
{
	auto dst = subtract(ray.origin, center);
	float b = dot(dst, ray.direction);
	float c = dot(dst, dst) - sq(radius);
	float d = sq(b) - c;
    
    if(d < 0.0f)
        return false; // not hit

    float sqrtd = sqrtf(d);
    dist1 = -b - sqrtd;
    dist2 = -b + sqrtd;

    return true;
}

inline bool Sphere::intersects(const Ray & ray, float minDistance, float maxDistance) const
{
    float dist1, dist2;
    if(!intersectHelper(ray, dist1, dist2))
        return false;

    auto minmax = std::minmax(dist1, dist2);

    return (minmax.first >= minDistance && minmax.first <= maxDistance)
        || (minmax.second >= minDistance && minmax.second <= maxDistance);
}

inline bool Sphere::findIntersection(const Ray & ray, float minDistance, RayIntersection & intersection) const
{
    float dist1, dist2;
    if(!intersectHelper(ray, dist1, dist2))
        return false;

#if 1 // TODO: check that this is equivalent
    bool bad1 = dist1 < minDistance;
    bool bad2 = dist2 < minDistance;
    if(bad1) dist1 = dist2;
    if(bad2) return false;
#else
    if(dist2 < dist1) { std::swap(dist1, dist2); }  // TODO - is this necessary?
    if(dist2 < minDistance) { return false; }
    if(dist1 < minDistance) { dist1 = dist2; }
#endif
    
    intersection.distance = dist1;
    // compute intersection position
    intersection.position = add(ray.origin, scale(ray.direction, intersection.distance));
    // compute surface normal
    intersection.normal = subtract(intersection.position, center).normalized();
    // generate tangent / bitangent
    coordinate::coordinateSystem(intersection.normal, intersection.tangent, intersection.bitangent);
    intersection.material = material;
    return true;
}

inline Slab Sphere::boundingBox()
{
    Direction3 d(radius, radius, radius);
    return Slab(center - d, center + d);
}

