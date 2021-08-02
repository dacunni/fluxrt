#ifndef _BRDF_H_
#define _BRDF_H_

#include "radiometry.h"
#include "vec2.h"
#include "vectortypes.h"

struct brdfSample {
    Direction3 Wo;
    float pdf;
};

namespace brdf {

InverseSteradians lambertian(const Direction3 & wi,
                             const Direction3 & wo,
                             const Direction3 & N);

InverseSteradians phong(const Direction3 & wi,
                        const Direction3 & wo,
                        const Direction3 & N,
                        const float a);       // exponent

// Importance Sampling

brdfSample samplePhong(const vec2 & e,        // random samples
                       const Direction3 & wi,
                       const Direction3 & N,
                       const float a);        // exponent

}; // brdf

#endif
