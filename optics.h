#ifndef _OPTICS_H_
#define _OPTICS_H_

namespace optics {

// Snell's Law : Angle formulation
//   NaN returned for total internal reflection
float snellsLawAngle(float n_i, float angle_i, float n_o);
// Snell's Law : Sine formulation
float snellsLawSine(float n_i, float sin_i, float n_o);

}; // optics

#endif
