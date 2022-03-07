#ifndef __VEC2_H__
#define __VEC2_H__

#include <string>
#include <iosfwd>
#include "base.h"

struct vec2
{
    inline vec2() = default;
    inline vec2(const vec2 & a) : x(a.x), y(a.y) {}
	inline vec2(float xn, float yn) : x(xn), y(yn) {}
    inline ~vec2() = default;

    static vec2 zero();

	inline void set(float xn, float yn) { x=xn; y=yn; }
	inline vec2 negated() const { return vec2(-x, -y); }
    inline void negate() { x = -x; y = -y; }

	inline float magnitude_sq() const { return sq(x) + sq(y); }
	inline float magnitude() const { return std::sqrt(sq(x) + sq(y)); }

	inline vec2 normalized() const;
    inline void normalize() { *this = this->normalized(); }

    inline float minElement() const { return std::min({x, y}); }
    inline float maxElement() const { return std::max({x, y}); }

    std::string string() const;

    float x = 0.0f;
    float y = 0.0f;
};

inline bool operator==(const vec2 & a, const vec2 & b) { return a.x == b.x && a.y == b.y; }

inline vec2 add(const vec2 & a, const vec2 & b) { return vec2(a.x + b.x, a.y + b.y); }
inline vec2 operator+(const vec2 & a, const vec2 & b) { return add(a, b); }

inline vec2 subtract(const vec2 & a, const vec2 & b) { return vec2(a.x - b.x, a.y - b.y); }
inline vec2 operator-(const vec2 & a, const vec2 & b) { return subtract(a, b); }
inline vec2 operator-(const vec2 & a) { return a.negated(); }

inline vec2 scale(const vec2 & a, float s) { return vec2(a.x * s, a.y * s); }
inline vec2 operator*(const vec2 & a, float s) { return scale(a, s); }
inline vec2 operator*(float s, const vec2 & a) { return scale(a, s); }
inline vec2 operator/(const vec2 & a, float s) { return scale(a, 1.0f / s); }

inline float dot(const vec2 & a, const vec2 & b) { return a.x * b.x + a.y * b.y; }
inline float clampedDot(const vec2 & a, const vec2 & b) { return std::max(dot(a, b), 0.0f); }

inline vec2 blend(const vec2 & a, float s, const vec2 & b, float t);
inline vec2 interp(const vec2 & a, const vec2 & b, const float alpha);

std::ostream & operator<<(std::ostream & os, const vec2 & v);

#include "vec2.hpp"
#endif
