#ifndef __VEC3_H__
#define __VEC3_H__

#include <cmath>
#include "base.h"

struct vec3
{
    inline vec3() = default;
    inline vec3(const vec3 & a) : x(a.x), y(a.y), z(a.z) {}
	inline vec3(float xn, float yn, float zn) : x(xn), y(yn), z(zn) {}
    inline ~vec3() = default;

	inline void set(float xn, float yn, float zn) { x=xn; y=yn; z=zn; }
	inline vec3 negated() const { return vec3(-x, -y, -z); }

	inline float magnitude_sq() const { return sq(x) + sq(y) + sq(z); }
	inline float magnitude() const { return std::sqrt(sq(x) + sq(y) + sq(z)); }

	inline vec3 normalized() const;
    inline void normalize() { *this = this->normalized(); }

    std::string string() const;

    static vec3 zero();

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

inline bool operator==(const vec3 & a, const vec3 & b) { return a.x == b.x && a.y == b.y && a.z == b.z; }

inline vec3 add(const vec3 & a, const vec3 & b) { return vec3(a.x + b.x, a.y + b.y, a.z + b.z); }
inline vec3 operator+(const vec3 & a, const vec3 & b) { return add(a, b); }

inline vec3 subtract(const vec3 & a, const vec3 & b) { return vec3(a.x - b.x, a.y - b.y, a.z - b.z); }
inline vec3 operator-(const vec3 & a, const vec3 & b) { return subtract(a, b); }

inline vec3 scale(const vec3 & a, float s) { return vec3(a.x * s, a.y * s, a.z * s); }
inline vec3 operator*(const vec3 & a, float s) { return scale(a, s); }
inline vec3 operator*(float s, const vec3 & a) { return scale(a, s); }

inline float dot(const vec3 & a, const vec3 & b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline float clampedDot(const vec3 & a, const vec3 & b) { return std::max(dot(a, b), 0.0f); }

inline vec3 cross(const vec3 & a, const vec3 & b);
inline vec3 blend(const vec3 & a, float s, const vec3 & b, float t);
inline vec3 mirror(const vec3 & a, const vec3 & n);
inline vec3 refract(const vec3 & a, const vec3 & n, float n1, float n2);
inline vec3 interp(const vec3 & a, const vec3 & b, const float alpha);

#include "vec3.hpp"
#endif
