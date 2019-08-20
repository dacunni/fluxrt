#ifndef _BRDF_H_
#define _BRDF_H_

#include "radiometry.h"

struct Direction3;

namespace brdf {
using namespace radiometry;

using BrdfFunction = radiometry::InverseSteradians (const Direction3 & /* wi */,
                                                    const Direction3 & /* wo */,
                                                    const Direction3 & /* N */);

BrdfFunction lambertian;

}; // brdf

#endif
