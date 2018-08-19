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

inline void quaternion::set(float xn, float yn, float zn, float rn)
{
	x = xn;
	y = yn;
	z = zn;
	r = rn;
}

inline void mult(const quaternion & q, const quaternion & p, quaternion & r)
{
    // IMPLEMENT ME
}

inline quaternion mult(const quaternion & q, const quaternion & p)
{
    quaternion r;
    // IMPLEMENT ME
    return r;
}

inline void rotate(const quaternion & q, const vec3 & v, vec3 & r)
{
    // IMPLEMENT ME
}

inline quaternion rotate(const quaternion & q, const vec3 & v)
{
    quaternion r;
    // IMPLEMENT ME
    return r;
}

#endif
