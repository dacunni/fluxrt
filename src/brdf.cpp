#include "vectortypes.h"
#include "constants.h"
#include "brdf.h"

using namespace constants;

namespace brdf {

InverseSteradians lambertian(const Direction3 & wi, const Direction3 & wo, const Direction3 & N)
{
    return 1.0f / PI;
}

InverseSteradians phong(const Direction3 & wi,
                        const Direction3 & wo,
                        const Direction3 & N,
                        float a)
{
    // TODO[DAC]: References show both of these as possible normalizing factors.
    //            Both seem to conserve energy. Not sure which is best.
    float normFactor = (a + 1.0) / TWO_PI;
    //float normFactor = (a + 2.0) / TWO_PI;
    return normFactor * std::pow(clampedDot(mirror(wi, N), wo), a);
}

}; // brdf

