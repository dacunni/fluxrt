#include <cmath>
#include <algorithm>
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

