#ifndef _OPTICS_H_
#define _OPTICS_H_

#include "ValueRGB.h"

namespace optics {

// Snell's Law : Angle formulation
//   NaN returned for total internal reflection
float snellsLawAngle(float n_i, float angle_i, float n_o);
// Snell's Law : Sine formulation
float snellsLawSine(float n_i, float sin_i, float n_o);

// Beer's Law attenuation at a given distance for a substance with a constant
// attenuation coefficient. This is a unitless quantity that relates
// transmitted radiant flux to incident radiant flux.
float beersLawAttenuation(float att, float dist);

ParameterRGB beersLawAttenuation(const ParameterRGB & att, float dist);

namespace ior {

// Sellmeier equation relating index of refraction to wavelength (in um)
float sellmeier(float wavelength, float B[], float C[], unsigned int num_coeffs);

}; // ior

}; // optics

#endif
