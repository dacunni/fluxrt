#include <cmath>
#include "constants.h"
#include "coordinate.h"

inline float RNG::uniform01()
{
    return distribution(engine);
}

inline float RNG::uniformRange(float min, float max)
{
	return uniform01() * (max - min) + min;
}

inline void RNG::uniformUnitCircle(float & x, float & y)
{
    uniformCircle(1.0f, x, y);
}

inline void RNG::uniformCircle(float radius, float & x, float & y)
{
    using namespace constants;

    float r = radius * std::sqrt(uniform01());
    float theta = uniformRange(0.0, 2.0 * PI);

    x = r * std::cos(theta);
    y = r * std::sin(theta);
}

inline void RNG::uniformSurfaceUnitSphere(float & x, float & y, float & z)
{
    using namespace constants;

    float u1 = uniform01();
    float u2 = uniform01();
    
    z = 1.0f - 2.0f * u1;
    float r = std::sqrt(std::max(0.0f, 1.0f - z * z));
    float phi = 2.0f * PI * u2;

    x = r * std::cos(phi);
    y = r * std::sin(phi);
}

inline void RNG::uniformSurfaceUnitSphere(vec3 & v)
{
    return uniformSurfaceUnitSphere(v.x, v.y, v.z);
}

inline void RNG::uniformSurfaceUnitHalfSphere(const Direction3 & halfSpace, vec3 & v)
{
    uniformSurfaceUnitSphere(v);

    // If we're on the wrong side, just flip to the right side
    if(dot(v, halfSpace) < 0.0f) {
        v.negate();
    }
}

inline void RNG::cosineAboutDirection(const Direction3 & n, vec3 & v)
{
    vec3 t, b;
    float x, y, z;

    coordinate::coordinateSystem(n, t, b);
    uniformUnitCircle(x, y);
    z = std::sqrt(1.0f - (x*x + y*y));

    v = x * t + y * b + z * n;
}



