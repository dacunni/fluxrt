#include <cmath>
#include "optics.h"

namespace optics {

// Snell's Law : Angle formulation
float snellsLawAngle(float n_i, float angle_i, float n_o)
{
    return asinf(n_i / n_o * sinf(angle_i));
}

// Snell's Law : Sine formulation
float snellsLawSine(float n_i, float sin_i, float n_o)
{
    return n_i / n_o * sin_i;
}

float beersLawAttenuation(float att, float dist)
{
    return std::exp(-att * dist);
}

}; // optics
