#ifndef __RNG_H__
#define __RNG_H__

// Random Number Generation

#include <random>
#include "base.h"

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

    std::random_device device;
    std::mt19937 engine;
    std::uniform_real_distribution<float> distribution;
};

#include "rng.hpp"
#endif
