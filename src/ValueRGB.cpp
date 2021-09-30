#include "ValueRGB.h"

#include <sstream>


std::string ColorRGB::string() const
{
    std::stringstream ss;
    const char * sep = ", ";
    ss << r << sep << g << sep << b;
    return ss.str();
}

