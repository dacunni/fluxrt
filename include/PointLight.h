#ifndef __POINT_LIGHT_H__
#define __POINT_LIGHT_H__

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

#endif
