#ifndef __QUATERNION_H__
#define __QUATERNION_H__

#include "vec3.h"

struct quaternion 
{
    inline quaternion() = default;
    inline quaternion(const quaternion & a);
    inline quaternion(float x, float y, float z, float r = 1.0f);
    inline quaternion(const vec3 & v, float r = 1.0f);
    inline ~quaternion() = default;

    inline void set(float x, float y, float z, float r);

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float r = 1.0f;
};

inline float norm(const quaternion & q);
inline quaternion conjugate(const quaternion & q);

inline quaternion mult(const quaternion & q, const quaternion & p);
inline quaternion rotate(const quaternion & q, const vec3 & v);

#include "quaternion.hpp"

#endif
