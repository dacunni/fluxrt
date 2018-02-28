#ifndef __QUATERNION_H__
#define __QUATERNION_H__

#include "vec3.h"

class quaternion 
{
public:
	inline quaternion();
	inline quaternion(const quaternion & a);
	inline quaternion(float x, float y, float z, float r = 1.0f);
	inline ~quaternion() {}
	
    inline void set(float x, float y, float z, float r);

    // IMPLEMENT ME
    inline quaternion conjugate();
    
    float x, y, z, r;
};

// quaternion math functions
//   Includes in-place and algebraic forms for convenience
inline void       mult(const quaternion & q, const quaternion & p, quaternion & r);
inline quaternion mult(const quaternion & q, const quaternion & p);
inline void       rotate(const quaternion & q, const vec3 & v, vec3 & r);
inline quaternion rotate(const quaternion & q, const vec3 & v);

#include "quaternion.hpp"

#endif
