#include "vectortypes.h"
#include "constants.h"
#include "coordinate.h"
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

brdfSample samplePhong(const vec2 & e,
                       const Direction3 & wi,
                       const Direction3 & N,
                       const float a)
{
    Direction3 mirrorDir = mirror(wi, N);
    Direction3 tangent, bitangent;
    coordinate::coordinateSystem(mirrorDir, tangent, bitangent);

    float theta = std::acos(std::pow(e.x, 1.0f / (a + 1.0f)));
    float phi = e.y * constants::TWO_PI;
    float sin_theta = std::sin(theta);

    Direction3 Wo = std::cos(theta) * mirrorDir +
        sin_theta * std::cos(phi) * tangent +
        sin_theta * std::sin(phi) * bitangent;

    brdfSample S;
    S.Wo = Wo;
    S.pdf = 1.0f; // TODO

    // TODO

    return S;
}

}; // brdf

