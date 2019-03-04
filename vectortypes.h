#ifndef __VECTORTYPES_H__
#define __VECTORTYPES_H__

#include "vec3.h"

struct Position3 : public vec3
{
    explicit Position3() : vec3() {}
    explicit Position3(const vec3 & a) : vec3(a) {}
	explicit Position3(float xn, float yn, float zn) : vec3(xn, yn, zn) {}
};

struct Direction3 : public vec3
{
    explicit Direction3() : vec3() {}
    explicit Direction3(const vec3 & a) : vec3(a) {}
	explicit Direction3(float xn, float yn, float zn) : vec3(xn, yn, zn) {}
	inline Direction3 normalized() const { return Direction3(vec3::normalized()); }
};

inline Position3 add(const Position3 & a, const Direction3 & b) { return Position3(add((vec3) a, (vec3) b)); }
inline Position3 operator+(const Position3 & a, const Direction3 & b) { return add(a, b); }

inline Direction3 add(const Direction3 & a, const Direction3 & b) { return Direction3(add((vec3) a, (vec3) b)); }
inline Direction3 operator+(const Direction3 & a, const Direction3 & b) { return add(a, b); }

inline Position3 subtract(const Position3 & a, const Direction3 & b) { return Position3(subtract((vec3) a, (vec3) b)); }
inline Position3 operator-(const Position3 & a, const Direction3 & b) { return subtract(a, b); }

inline Direction3 subtract(const Position3 & a, const Position3 & b) { return Direction3(subtract((vec3) a, (vec3) b)); }
inline Direction3 operator-(const Position3 & a, const Position3 & b) { return subtract(a, b); }

inline Direction3 subtract(const Direction3 & a, const Direction3 & b) { return Direction3(subtract((vec3) a, (vec3) b)); }
inline Direction3 operator-(const Direction3 & a, const Direction3 & b) { return subtract(a, b); }

inline Direction3 operator-(const Direction3 & a) { return Direction3(a.negated()); }

inline Direction3 scale(const Direction3 & a, float s) { return Direction3(scale((vec3)a, s)); }
inline Direction3 operator*(const Direction3 & a, float s) { return scale(a, s); }
inline Direction3 operator*(float s, const Direction3 & a) { return scale(a, s); }

inline Direction3 cross(const Direction3 & a, const Direction3 & b) { return Direction3(cross((vec3)a, (vec3)b)); }
inline Direction3 mirror(const Direction3 & a, const Direction3 & n) { return Direction3(mirror((vec3)a, (vec3)n)); }
inline Direction3 refract(const Direction3 & a, const Direction3 & n, float n1, float n2) { return Direction3(refract((vec3)a, (vec3)n, n1, n2)); }

#endif
