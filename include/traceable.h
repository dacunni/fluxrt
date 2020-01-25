#ifndef __TRACEABLE__
#define __TRACEABLE__

#include "transform.h"
#include "ray.h"

//
// Wrapper template for making generic traceable objects out of more
// basic traceable shapes. This template allows us to create derivatives
// that add functionality like having transforms applied to them without
// complicating the shape classes.
//
template<typename SHAPE>
struct Traceable
{
    inline Traceable() = default;
    inline Traceable(SHAPE && s) : shape(std::move(s)) { }
    inline ~Traceable() = default;

    SHAPE shape;
    Transform transform;
};

// Ray intersection
template<typename SHAPE>
inline bool intersects(const Ray & rayWorld, const Traceable<SHAPE> & obj, float minDistanceWorld);
template<typename SHAPE>
inline bool findIntersection(const Ray & rayWorld, const Traceable<SHAPE> & obj, float minDistanceWorld, RayIntersection & intersection);

// Inline implementations
template<typename SHAPE>
inline bool intersects(const Ray & rayWorld, const Traceable<SHAPE> & obj, float minDistanceWorld)
{
    // Transform the ray into object space
    Ray rayObj = Ray{
        obj.transform.rev * rayWorld.origin,
        (obj.transform.rev * rayWorld.direction).normalized()
    };

    // Calculate the minimum distance along the ray in object space
    Position3 minPositionWorld = rayWorld.pointAt(minDistanceWorld);
    Position3 minPositionObj = obj.transform.rev * minPositionWorld;
    float minDistanceObj = (minPositionObj - rayObj.origin).magnitude();

    return intersects(rayObj, obj.shape, minDistanceObj);
}

template<typename SHAPE>
inline bool findIntersection(const Ray & rayWorld, const Traceable<SHAPE> & obj, float minDistanceWorld, RayIntersection & intersection)
{
    // Transform the ray into object space
    Ray rayObj = Ray{
        obj.transform.rev * rayWorld.origin,
        (obj.transform.rev * rayWorld.direction).normalized()
    };

    // Calculate the minimum distance along the ray in object space
    Position3 minPositionWorld = rayWorld.pointAt(minDistanceWorld);
    Position3 minPositionObj = obj.transform.rev * minPositionWorld;
    float minDistanceObj = (minPositionObj - rayObj.origin).magnitude();

    // Do intersection in object space
    bool hit = findIntersection(rayObj, obj.shape, minDistanceObj, intersection);

    if(hit) {
        // Transform hit back to world space
        intersection.ray = rayWorld;
        intersection.position = obj.transform.fwd * intersection.position;
        // Normals and the like transform as the inverse transpose
        intersection.normal = multTranspose(obj.transform.rev, intersection.normal).normalized();
        intersection.tangent = multTranspose(obj.transform.rev, intersection.tangent).normalized();
        intersection.bitangent = multTranspose(obj.transform.rev, intersection.bitangent).normalized();
        // Get distance in world space
        intersection.distance = (intersection.position - rayWorld.origin).magnitude();
    }

    return hit;
}

#endif
