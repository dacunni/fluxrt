#ifndef __VEC4_H__
#define __VEC4_H__

#include <cmath>
#include "base.h"

struct vec4
{
    inline vec4() = default;
    inline vec4(const vec4 & a) : x(a.x), y(a.y), z(a.z), w(a.w) {}
	inline vec4(float xn, float yn, float zn, float wn = 1.0f) : x(xn), y(yn), z(zn), w(wn) {}
    inline ~vec4() = default;

	inline void set(float xn, float yn, float zn, float wn = 1.0f) { x=xn; y=yn; z=zn; w=wn;}
	inline vec4 negated() const { return vec4(-x, -y, -z, w); }

	inline float magnitude_sq() const;
    inline float magnitude() const;

    inline void normalize();
	inline vec4 normalized() const;

    static vec4 zero();

    std::string string() const;

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f;
};

inline vec4 scale(const vec4 & a, float s);

#include "vec4.hpp"
#endif
