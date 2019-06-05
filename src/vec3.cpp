#include <iostream>
#include <sstream>
#include "vec3.h"

vec3 vec3::zero() { return vec3(0.0f, 0.0f, 0.0f); }

std::string vec3::string() const
{
    std::stringstream ss;
    const char * sep = ", ";
    ss << x << sep << y << sep << z;
    return ss.str();
}

std::ostream & operator<<(std::ostream & os, const vec3 & v)
{
    const char * sep = ", ";
    os << v.x << sep << v.y << sep << v.z;
    return os;
}

