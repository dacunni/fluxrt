#ifndef __VEC3_H__
#define __VEC3_H__

#include <string>
#include <iosfwd>
#include "base.h"

struct vec3
{
    inline vec3() = default;
    inline vec3(const vec3 & a) : x(a.x), y(a.y), z(a.z) {}
	inline vec3(float xn, float yn, float zn) : x(xn), y(yn), z(zn) {}
    inline vec3(float v[3]) : x(v[0]), y(v[1]), z(v[2]) {}
    inline ~vec3() = default;

    static vec3 zero();

	inline void set(float xn, float yn, float zn) { x=xn; y=yn; z=zn; }
	inline vec3 negated() const { return vec3(-x, -y, -z); }
    inline void negate() { x = -x; y = -y; z = -z; }

	inline float magnitude_sq() const { return sq(x) + sq(y) + sq(z); }
	inline float magnitude() const { return std::sqrt(sq(x) + sq(y) + sq(z)); }

	inline vec3 normalized() const;
    inline void normalize() { *this = this->normalized(); }

    inline vec3 invertedComponents() const { return { 1.0f / x, 1.0f / y, 1.0f / z }; }

    inline float minElement() const { return std::min({x, y, z}); }
    inline float maxElement() const { return std::max({x, y, z}); }

    std::string string() const;

    bool isZeros() const { return x == 0.0f && y == 0.0f && z == 0.0f; }
    bool isOnes() const { return x == 1.0f && y == 1.0f && z == 1.0f; }
    bool hasNaN() const { return std::isnan(x) || std::isnan(y) || std::isnan(z); }
    bool hasInf() const { return std::isinf(x) || std::isinf(y) || std::isinf(z); }

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

inline bool operator==(const vec3 & a, const vec3 & b) { return a.x == b.x && a.y == b.y && a.z == b.z; }

inline vec3 add(const vec3 & a, const vec3 & b) { return vec3(a.x + b.x, a.y + b.y, a.z + b.z); }
inline vec3 operator+(const vec3 & a, const vec3 & b) { return add(a, b); }

inline vec3 subtract(const vec3 & a, const vec3 & b) { return vec3(a.x - b.x, a.y - b.y, a.z - b.z); }
inline vec3 operator-(const vec3 & a, const vec3 & b) { return subtract(a, b); }
inline vec3 operator-(const vec3 & a) { return a.negated(); }

inline vec3 scale(const vec3 & a, float s) { return vec3(a.x * s, a.y * s, a.z * s); }
inline vec3 operator*(const vec3 & a, float s) { return scale(a, s); }
inline vec3 operator*(float s, const vec3 & a) { return scale(a, s); }
inline vec3 operator/(const vec3 & a, float s) { return scale(a, 1.0f / s); }

inline float dot(const vec3 & a, const vec3 & b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline float absDot(const vec3 & a, const vec3 & b) { return std::abs(a.x * b.x + a.y * b.y + a.z * b.z); }
inline float clampedDot(const vec3 & a, const vec3 & b) { return std::max(dot(a, b), 0.0f); }

inline vec3 cross(const vec3 & a, const vec3 & b);
inline vec3 blend(const vec3 & a, float s, const vec3 & b, float t);
inline vec3 mirror(const vec3 & a, const vec3 & n);
inline vec3 refract(const vec3 & a, const vec3 & n, float n1, float n2);
inline vec3 interp(const vec3 & a, const vec3 & b, const float alpha);

std::ostream & operator<<(std::ostream & os, const vec3 & v);

#include "vec3.hpp"
#endif
