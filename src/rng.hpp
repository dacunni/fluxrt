#include <cmath>
#include "constants.h"

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

void RNG::uniformSurfaceUnitSphere(float & x, float & y, float & z)
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


