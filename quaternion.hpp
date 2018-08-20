#ifndef __QUATERNION_HPP__
#define __QUATERNION_HPP__

#include <cmath>

inline quaternion::quaternion(const quaternion & a)
{
	x = a.x;
	y = a.y;
	z = a.z;
	r = a.r;
}

inline quaternion::quaternion(float xn, float yn, float zn, float rn)
{
    set(xn, yn, zn, rn);
}

inline quaternion::quaternion(const vec3 & v, float rn)
{
    set(v.x, v.y, v.z, rn);
}

inline void quaternion::set(float xn, float yn, float zn, float rn)
{
	x = xn;
	y = yn;
	z = zn;
	r = rn;
}

inline float norm(const quaternion & q)
{
    return std::sqrt(sq(q.x) + sq(q.y) + sq(q.z) + sq(q.r));
}

inline quaternion conjugate(const quaternion & q)
{
    return quaternion(-q.x, -q.y, -q.z, q.r);
}

inline quaternion mult(const quaternion & q, const quaternion & p)
{
    const float s1 = q.r;
    const float s2 = p.r;
    const vec3 v1 { q.x, q.y, q.z };
    const vec3 v2 { p.x, p.y, p.z };
    return quaternion(s1 * v2 + s2 * v1 + cross(v1, v2),
                      s1 * s2 - dot(v1, v2));
}

inline quaternion rotate(const quaternion & q, const vec3 & v)
{
    quaternion r;
    // IMPLEMENT ME
    return r;
}

#endif
