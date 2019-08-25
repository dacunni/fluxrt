#ifndef _BRDF_H_
#define _BRDF_H_

#include "radiometry.h"

struct Direction3;

namespace brdf {
using namespace radiometry;

radiometry::InverseSteradians lambertian(const Direction3 & wi,
                                         const Direction3 & wo,
                                         const Direction3 & N);

radiometry::InverseSteradians phong(const Direction3 & wi,
                                    const Direction3 & wo,
                                    const Direction3 & N,
                                    const float a); // exponent

}; // brdf

#endif
