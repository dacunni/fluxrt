#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include <cmath>

namespace constants {

    static constexpr double PI = M_PI;

};

constexpr double DegreesToRadians(double d) { return d * constants::PI / 180.0; }

#endif
