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

	inline float magnitude_sq() const {
        if(w == 0.0f) return sq(x) + sq(y) + sq(z);
        else return sq(x/w) + sq(y/w) + sq(z/w);
    }
	inline float magnitude() const { return std::sqrt(sq(x) + sq(y) + sq(z)); }

    inline void normalize() {
        float magsq = magnitude_sq();
        if(magsq != 0.0f) {
            float mag = std::sqrt(magsq);
            float invmag = 1.0f / mag;
            x *= invmag; y *= invmag; z *= invmag;
        }
    }

	inline vec4 normalized() const {
        float magsq = magnitude_sq();
        if(magsq != 0.0f) {
            float mag = std::sqrt(magsq);
            float invmag = 1.0f / mag;
            return vec4(x * invmag, y * invmag, z * invmag, w);
        }
        return *this;
    }

    static vec4 zero();

    std::string string() const;

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f;
};

inline vec4 scale(const vec4 & a, float s) {
    return vec4(a.x * s, a.y * s, a.z * s, a.w);
}

#endif
