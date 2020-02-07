#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "vectortypes.h"
#include "radiometry.h"

struct PointLight
{
	inline PointLight() = default;
	inline PointLight(const Position3 & p, const radiometry::RadianceRGB & i) : position(p), intensity(i) {}
	inline ~PointLight() = default;

	Position3 position = Position3{ 0.0f, 0.0f, 0.0f };
    radiometry::RadianceRGB intensity = { 1.0f, 1.0f, 1.0f };
};

#endif
