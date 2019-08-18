#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include <cmath>

namespace constants {

    static constexpr double PI      = M_PI;
    static constexpr double TWO_PI  = 2.0 * PI;
    static constexpr double FOUR_PI = 4.0 * PI;

};

constexpr double DegreesToRadians(double d) { return d * constants::PI / 180.0; }
constexpr double RadiansToDegrees(double r) { return r * 180.0 / constants::PI; }

#endif
