#ifndef __IES_LIGHT_H__
#define __IES_LIGHT_H__

#include <memory>
#include "vectortypes.h"
#include "radiometry.h"
#include "IESProfile.h"

// IES Light — a point light whose intensity distribution follows an IES profile.
//
// The fixture's "down" axis is given by `direction`. Vertical angle 0 in the
// profile corresponds to the direction vector (nadir), 90° is perpendicular to
// it, and 180° is directly opposite (zenith).
//
// `color` is a tint / scale applied to the candela values. Use white (1,1,1)
// for unmodified photometric data, or scale to control overall brightness.
//
// The candela values from the profile are used like a point light (1/r²
// fall-off) and the renderer multiplies in the BRDF and cos(theta) at the
// shading point.
struct IESLight
{
    IESLight() = default;
    IESLight(const Position3 & pos, const Direction3 & dir,
             const RadianceRGB & color,
             std::shared_ptr<IESProfile> profile)
        : position(pos), direction(dir), color(color), profile(std::move(profile)) {}

    Position3 position  = Position3{0.0f, 0.0f, 0.0f};
    Direction3 direction = Direction3{0.0f, -1.0f, 0.0f};  // fixture "down"
    RadianceRGB color   = {1.0f, 1.0f, 1.0f};
    std::shared_ptr<IESProfile> profile;
};

#endif
