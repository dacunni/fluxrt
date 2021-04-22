#ifndef __TRACEABLE__
#define __TRACEABLE__

#include "transform.h"
#include "Ray.h"

// Base class for traceable objects
struct Traceable
{
    inline Traceable() = default;
    virtual inline ~Traceable() = default;

    // Ray intersection interface
    inline bool intersectsWorldRay(const Ray & rayWorld, float minDistanceWorld, float maxDistanceWorld) const;
    inline bool findIntersectionWorldRay(const Ray & rayWorld, float minDistanceWorld, RayIntersection & intersection) const;

    // Ray intersection implementation
    virtual bool intersects(const Ray & ray, float minDistance, float maxDistance) const = 0;
    virtual bool findIntersection(const Ray & ray, float minDistance, RayIntersection & intersection) const = 0;

    Transform transform;
};

// Inline implementations
inline bool Traceable::intersectsWorldRay(const Ray & rayWorld, float minDistanceWorld, float maxDistanceWorld) const
{
    // Transform the ray into object space
    Ray rayObj = Ray{
        transform.rev * rayWorld.origin,
        (transform.rev * rayWorld.direction).normalized()
    };

    // Calculate the minimum distance along the ray in object space
    Position3 minPositionWorld = rayWorld.pointAt(minDistanceWorld);
    Position3 minPositionObj = transform.rev * minPositionWorld;
    float minDistanceObj = (minPositionObj - rayObj.origin).magnitude();

    // Calculate the maximum distance along the ray in object space
    Position3 maxPositionWorld = rayWorld.pointAt(maxDistanceWorld);
    Position3 maxPositionObj = transform.rev * maxPositionWorld;
    float maxDistanceObj = (maxPositionObj - rayObj.origin).magnitude();

    return intersects(rayObj, minDistanceObj, maxDistanceObj);
}

inline bool Traceable::findIntersectionWorldRay(const Ray & rayWorld, float minDistanceWorld, RayIntersection & intersection) const
{
    // Transform the ray into object space
    Ray rayObj = Ray{
        transform.rev * rayWorld.origin,
        (transform.rev * rayWorld.direction).normalized()
    };

    // Calculate the minimum distance along the ray in object space
    Position3 minPositionWorld = rayWorld.pointAt(minDistanceWorld);
    Position3 minPositionObj = transform.rev * minPositionWorld;
    float minDistanceObj = (minPositionObj - rayObj.origin).magnitude();

    // Do intersection in object space
    bool hit = findIntersection(rayObj, minDistanceObj, intersection);

    if(hit) {
        // Transform hit back to world space
        intersection.ray = rayWorld;
        intersection.position = transform.fwd * intersection.position;
        // Normals and the like transform as the inverse transpose
        intersection.normal = multTranspose(transform.rev, intersection.normal).normalized();
        intersection.tangent = multTranspose(transform.rev, intersection.tangent).normalized();
        intersection.bitangent = multTranspose(transform.rev, intersection.bitangent).normalized();
        // Get distance in world space
        intersection.distance = (intersection.position - rayWorld.origin).magnitude();
    }

    return hit;
}

#endif
