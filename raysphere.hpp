#include <cmath>
#include <sstream>

inline bool intersectHelper(const Ray & ray, const Sphere & sphere, float & dist1, float & dist2)
{
	auto dst = subtract(ray.origin, sphere.center);
	float b = dot(dst, ray.direction);
	float c = dot(dst, dst) - sq(sphere.radius);
	float d = sq(b) - c;
    
    if(d < 0.0f)
        return false; // not hit

    float sqrtd = sqrtf(d);
    dist1 = -b - sqrtd;
    dist2 = -b + sqrtd;

    return true;
}

inline bool intersects(const Ray & ray, const Sphere & sphere, float minDistance)
{
    float dist1, dist2;
    if(!intersectHelper(ray, sphere, dist1, dist2))
        return false;

    return dist1 >= minDistance || dist2 >= minDistance;
}

inline bool findIntersection(const Ray & ray, const Sphere & sphere, float minDistance, RayIntersection & intersection)
{
    float dist1, dist2;
    if(!intersectHelper(ray, sphere, dist1, dist2))
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
    intersection.normal = subtract(intersection.position, sphere.center).normalized();
    return true;
}

