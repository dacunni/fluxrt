#ifndef _BRDF_H_
#define _BRDF_H_

#include "radiometry.h"
#include "vec2.h"
#include "vectortypes.h"

struct brdfSample {
    Direction3 W;
    float pdf;
};

namespace brdf {

InverseSteradians lambertian(const Direction3 & Wi,
                             const Direction3 & Wo,
                             const Direction3 & N);

InverseSteradians phong(const Direction3 & Wi,
                        const Direction3 & Wo,
                        const Direction3 & N,
                        const float a);       // exponent

// Importance Sampling

brdfSample samplePhong(const vec2 & e,        // random samples
                       const Direction3 & Wi,
                       const Direction3 & N,
                       const float a);        // exponent

}; // brdf

#endif
