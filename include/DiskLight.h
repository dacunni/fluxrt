#ifndef __DISK_LIGHT_H__
#define __DISK_LIGHT_H__

#include "traceable.h"
#include "vectortypes.h"
#include "radiometry.h"

// Disk Light (diffuse)
//   Centered at the origin. Oriented facing +/-Y.
//   Radiates from both faces
struct DiskLight : public Traceable
{
    inline DiskLight() = default;
    inline DiskLight(const Position3 & p, const Direction3 & d, float r,
                     MaterialID material)
        : position(p), direction(d), radius(r), material(material) {}
    inline ~DiskLight() = default;

    // Ray intersection implementation
    inline virtual bool intersects(const Ray & ray, float minDistance, float maxDistance) const override;
    inline virtual bool findIntersection(const Ray & ray, float minDistance, RayIntersection & intersection) const override;

	Position3 position = Position3{ 0.0f, 0.0f, 0.0f };
    Direction3 direction = Direction3{ 0.0f, -1.0f, 0.0f };
    float radius = 1.0f;
    MaterialID material = NoMaterial;
};

#include "DiskLight.hpp"
#endif
