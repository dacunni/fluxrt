#include <iostream>
#include <sstream>
#include "vec2.h"

vec2 vec2::zero() { return vec2(0.0f, 0.0f); }

std::string vec2::string() const
{
    std::stringstream ss;
    const char * sep = ", ";
    ss << x << sep << y;
    return ss.str();
}

std::ostream & operator<<(std::ostream & os, const vec2 & v)
{
    const char * sep = ", ";
    os << v.x << sep << v.y;
    return os;
}

