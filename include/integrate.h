#ifndef _INTEGRATE_H_
#define _INTEGRATE_H_

#include <functional>

// Conventions
//
// All 3D coordinate systems are assumed to be right handed unless otherwise noted
//
// 3D Polar Coordinates (Hemisphere)
//   theta : angle from +Z axis
//   phi   : angle about +Z axis starting from +X axis and increasing towards +Y (CCW in right handed system)
//
// 3D Polar Coordinates (Sphere)
//   theta : angle from +Z axis
//   phi   : angle about +Z axis starting from +X axis and increasing towards +Y (CCW in right handed system)

namespace integrate {

using FunctionOverHemisphere = float(float /* theta */, float /* phi */);
using FunctionOverSphere = float(float /* theta */, float /* phi */);

float overUnitHemisphere(std::function<FunctionOverHemisphere> f,
                         unsigned int thetaSteps, unsigned int phiSteps);
float overUnitSphere(std::function<FunctionOverHemisphere> f,
                     unsigned int thetaSteps, unsigned int phiSteps);

}; // integrate

#endif
