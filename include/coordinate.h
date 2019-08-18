#ifndef _COORDINATE_H_
#define _COORDINATE_H_

#include <cmath>
#include "vec3.h"

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

namespace coordinate {

// 3D Euclidean <-> Polar
inline void euclideanToPolar(float x, float y, float z,
                             float & theta, float & phi, float & radius);
inline void polarToEuclidean(float theta, float phi, float radius,
                             float & x, float & y, float & z);

// Build an ad hoc coordiante system about a vector
inline void coordinateSystem(const vec3 & v1, vec3 & v2, vec3 & v3);

// Inline Definitions

inline void euclideanToPolar(float x, float y, float z,
                             float & theta, float & phi, float & radius)
{
    radius = std::sqrt(x * x + y * y + z * z);
    theta = std::acos(z / radius);            // [0, pi]
    phi = std::atan2(y / radius, x / radius); // [-pi, pi]
}

inline void polarToEuclidean(float theta, float phi, float radius,
                             float & x, float & y, float & z)
{
    const float cosTheta = std::cos(theta);
    const float sinTheta = std::sin(theta);
    const float cosPhi   = std::cos(phi);
    const float sinPhi   = std::sin(phi);

    x = radius * sinTheta * cosPhi;
    y = radius * sinTheta * sinPhi;
    z = radius * cosTheta;
}

// Based on the implementation in pbrt-v3
inline void coordinateSystem(const vec3 & v1, vec3 & v2, vec3 & v3)
{
    if (std::abs(v1.x) > std::abs(v1.y)) {
        auto d = std::sqrt(v1.x * v1.x + v1.z * v1.z);
        v2 = vec3(-v1.z, 0, v1.x) / d;
    }
    else {
        auto d = std::sqrt(v1.y * v1.y + v1.z * v1.z);
        v2 = vec3(0, v1.z, -v1.y) / d;
    }

    v3 = cross(v1, v2);
}


}; // coordinate

#endif
