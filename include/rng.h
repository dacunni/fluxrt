#ifndef __RNG_H__
#define __RNG_H__

// Random Number Generation

#include <random>
#include "base.h"
#include "vectortypes.h"
#include "vec2.h"

struct RNG
{
    RNG();
    ~RNG() = default;

    // 1D
	inline float uniform01();
	inline float uniformRange(float min, float max);

    // 2D
    inline void uniformUnitCircle(float & x, float & y);
    inline vec2 uniformUnitCircle();
    inline void uniformCircle(float radius, float & x, float & y);
    inline vec2 uniformCircle(float radius);
    inline void uniformRectangle(float xmin, float xmax,
                                 float ymin, float ymax,
                                 float & x, float & y);
    inline void uniformRectangle(float xmin, float xmax,
                                 float ymin, float ymax,
                                 vec2 & v);
    inline vec2 uniformRectangle(float xmin, float xmax,
                                 float ymin, float ymax);
    inline vec2 uniform2DRange01();
    inline vec2 uniform2DRange01Stratified(int xbins, int ybins, int index);

    // 3D
	inline void uniformSurfaceUnitSphere(float & x, float & y, float & z);
	inline void uniformSurfaceUnitSphere(vec3 & v);
	inline vec3 uniformSurfaceUnitSphere();
    inline void uniformSurfaceUnitHalfSphere(const Direction3 & halfSpace, vec3 & v);
    inline vec3 uniformSurfaceUnitHalfSphere(const Direction3 & halfSpace);
    inline void cosineAboutDirection(const Direction3 & n, vec3 & v);
    inline vec3 cosineAboutDirection(const Direction3 & n);

    std::random_device device;
    std::mt19937 engine;
    //std::minstd_rand0 engine;
    //std::default_random_engine engine;
    std::uniform_real_distribution<float> distribution;
};

#include "rng.hpp"
#endif
