#ifndef __RNG_H__
#define __RNG_H__

// Random Number Generation

#include <random>
#include "base.h"
#include "vectortypes.h"

struct RNG
{
    RNG();
    ~RNG() = default;

    // 1D
	inline float uniform01();
	inline float uniformRange(float min, float max);

    // 2D
    inline void uniformUnitCircle(float & x, float & y);
    inline void uniformCircle(float radius, float & x, float & y);

    // 3D
	inline void uniformSurfaceUnitSphere(float & x, float & y, float & z);
	inline void uniformSurfaceUnitSphere(vec3 & v);
    inline void uniformSurfaceUnitHalfSphere(const Direction3 & halfSpace, vec3 & v);
    inline void cosineAboutDirection(const Direction3 & n, vec3 & v);

    std::random_device device;
    std::mt19937 engine;
    //std::minstd_rand0 engine;
    //std::default_random_engine engine;
    std::uniform_real_distribution<float> distribution;
};

#include "rng.hpp"
#endif
