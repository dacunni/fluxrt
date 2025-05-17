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

inline float RNG::gaussian(float mean, float stddev)
{
    return mean + normalDistribution(engine) * stddev;
}

inline float RNG::gaussian(float stddev)
{
    return normalDistribution(engine) * stddev;
}

inline vec2 RNG::uniformCircle(const vec2 & e, float radius)
{
    using namespace constants;

    float r = radius * std::sqrt(e.x);
    float theta = 2.0f * float(PI) * e.y;

    return vec2 {
        r * std::cos(theta),
        r * std::sin(theta)
    };
}

inline vec2 RNG::uniformUnitCircle(const vec2 & e)
{
    return uniformCircle(e, 1.0f);
}

inline void RNG::uniformUnitCircle(float & x, float & y)
{
    uniformCircle(1.0f, x, y);
}

inline vec2 RNG::uniformUnitCircle()
{
    vec2 p;
    uniformCircle(1.0f, p.x, p.y);
    return p;
}

inline void RNG::uniformCircle(float radius, float & x, float & y)
{
    using namespace constants;

    float r = radius * std::sqrt(uniform01());
    float theta = uniformRange(0.0f, 2.0f * float(PI));

    x = r * std::cos(theta);
    y = r * std::sin(theta);
}

inline vec2 RNG::uniformCircle(float radius)
{
    vec2 point;
    uniformCircle(radius, point.x, point.y);
    return point;
}

inline void RNG::uniformRectangle(float xmin, float xmax,
                                  float ymin, float ymax,
                                  float & x, float & y)
{
    x = uniformRange(xmin, xmax);
    y = uniformRange(ymin, ymax);
}

inline void RNG::uniformRectangle(float xmin, float xmax,
                                  float ymin, float ymax,
                                  vec2 & v)
{
    uniformRectangle(xmin, xmax, ymin, ymax, v.x, v.y);
}

inline vec2 RNG::uniformRectangle(float xmin, float xmax,
                                  float ymin, float ymax)
{
    return {
        uniformRange(xmin, xmax),
        uniformRange(ymin, ymax)
    };
}

inline vec2 RNG::uniform2DRange01()
{
    return { 
        uniform01(),
        uniform01()
    };
}

inline vec2 RNG::uniform2DRange01Stratified(int xbins, int ybins, int index)
{
    int xbin = index % xbins;
    int ybin = (index / xbins) % ybins;

    return {
        uniformRange(xbin, xbin + 1) / float(xbins),
        uniformRange(ybin, ybin + 1) / float(ybins)
    };
}

inline vec2 RNG::gaussian2D(float mean, float stddev)
{
    return { gaussian(mean, stddev), gaussian(mean, stddev) };
}

inline vec2 RNG::gaussian2D(float stddev)
{
    return { gaussian(stddev), gaussian(stddev) };
}

inline vec3 RNG::uniformSurfaceUnitSphere(const vec2 & e)
{
    using namespace constants;

    float u1 = e.x;
    float u2 = e.y;
    vec3 v;
    
    v.z = 1.0f - 2.0f * u1;
    float r = std::sqrt(std::max(0.0f, 1.0f - v.z * v.z));
    float phi = 2.0f * float(PI) * u2;

    v.x = r * std::cos(phi);
    v.y = r * std::sin(phi);

    return v;
}

inline void RNG::uniformSurfaceUnitSphere(float & x, float & y, float & z)
{
    using namespace constants;

    float u1 = uniform01();
    float u2 = uniform01();
    
    z = 1.0f - 2.0f * u1;
    float r = std::sqrt(std::max(0.0f, 1.0f - z * z));
    float phi = 2.0f * float(PI) * u2;

    x = r * std::cos(phi);
    y = r * std::sin(phi);
}

inline void RNG::uniformSurfaceUnitSphere(vec3 & v)
{
    return uniformSurfaceUnitSphere(v.x, v.y, v.z);
}

inline vec3 RNG::uniformSurfaceUnitSphere()
{
    vec3 v;
    uniformSurfaceUnitSphere(v);
    return v;
}

inline vec3 RNG::uniformSurfaceUnitHalfSphere(const vec2 & e, const Direction3 & halfSpace)
{
    vec3 v = uniformSurfaceUnitSphere(e);

    // If we're on the wrong side, just flip to the right side
    if(dot(v, halfSpace) < 0.0f) {
        v.negate();
    }

    return v;
}

inline void RNG::uniformSurfaceUnitHalfSphere(const Direction3 & halfSpace, vec3 & v)
{
    uniformSurfaceUnitSphere(v);

    // If we're on the wrong side, just flip to the right side
    if(dot(v, halfSpace) < 0.0f) {
        v.negate();
    }
}

inline vec3 RNG::uniformSurfaceUnitHalfSphere(const Direction3 & halfSpace)
{
    vec3 v;
    uniformSurfaceUnitHalfSphere(halfSpace, v);
    return v;
}

inline vec3 RNG::cosineAboutDirection(const vec2 & e, const Direction3 & n)
{
    vec3 t, b;
    float z;

    coordinate::coordinateSystem(n, t, b);
    vec2 v = uniformUnitCircle(e);
    z = std::sqrt(1.0f - (v.x * v.x + v.y * v.y));

    return v.x * t + v.y * b + z * n;
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

inline vec3 RNG::cosineAboutDirection(const Direction3 & n)
{
    vec3 t, b;
    float x, y, z;

    coordinate::coordinateSystem(n, t, b);
    uniformUnitCircle(x, y);
    z = std::sqrt(1.0f - (x*x + y*y));

    return x * t + y * b + z * n;
}

inline vec3 RNG::gaussian3D(float mean, float stddev)
{
    return { gaussian(mean, stddev), gaussian(mean, stddev), gaussian(mean, stddev) };
}

inline vec3 RNG::gaussian3D(float stddev)
{
    return { gaussian(stddev), gaussian(stddev), gaussian(stddev) };
}


