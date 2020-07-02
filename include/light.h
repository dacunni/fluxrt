#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "vectortypes.h"
#include "radiometry.h"

// Point Light
//   Radiates equally in all directions
struct PointLight
{
	inline PointLight() = default;
	inline PointLight(const Position3 & p, const RadianceRGB & i) : position(p), intensity(i) {}
	inline ~PointLight() = default;

	Position3 position = Position3{ 0.0f, 0.0f, 0.0f };
    RadianceRGB intensity = { 1.0f, 1.0f, 1.0f };
};

// Disk Light (diffuse)
//   Centered at the origin. Oriented facing +/-Y.
//   Radiates from both faces
struct DiskLight
{
    inline DiskLight() = default;
    inline DiskLight(const Position3 & p, const Direction3 & d, float r,
                     MaterialID material)
        : position(p), direction(d), radius(r), material(material) {}
    inline ~DiskLight() = default;

	Position3 position = Position3{ 0.0f, 0.0f, 0.0f };
    Direction3 direction = Direction3{ 0.0f, -1.0f, 0.0f };
    float radius = 1.0f;
    MaterialID material = NoMaterial;
};

// Ray intersection
inline bool intersects(const Ray & ray, const DiskLight & light, float minDistance, float maxDistance);
inline bool findIntersection(const Ray & ray, const DiskLight & light, float minDistance, RayIntersection & intersection);

#include "light.hpp"
#endif
