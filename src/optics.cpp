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

ParameterRGB beersLawAttenuation(const ParameterRGB & att, float dist)
{
    return {
        beersLawAttenuation(att.r, dist),
        beersLawAttenuation(att.g, dist),
        beersLawAttenuation(att.b, dist),
    };
}

namespace ior {

float sellmeier(float wavelength, float B[], float C[], unsigned int num_coeffs)
{
    float n2 = 1.0f;
    float w2 = wavelength * wavelength;

    for(unsigned int i = 0; i < num_coeffs; ++i) {
        n2 += B[i] * w2 / (w2 - C[i]);
    }

    return std::sqrt(n2);
}

}; // ior

}; // optics
