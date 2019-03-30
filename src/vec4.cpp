#include <sstream>
#include "vec4.h"

vec4 vec4::zero() { return vec4(0.0f, 0.0f, 0.0f); }

std::string vec4::string() const
{
    std::stringstream ss;
    const char * sep = ", ";
    ss << x << sep << y << sep << z << sep << w;
    return ss.str();
}


