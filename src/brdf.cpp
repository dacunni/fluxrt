#include "vectortypes.h"
#include "constants.h"
#include "brdf.h"

using namespace constants;
using namespace radiometry;

namespace brdf {

radiometry::InverseSteradians lambertian(const Direction3 & wi, const Direction3 & wo, const Direction3 & N)
{
    return 1.0f / PI;
}

}; // brdf

