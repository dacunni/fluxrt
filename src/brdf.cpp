#include "vectortypes.h"
#include "constants.h"
#include "coordinate.h"
#include "brdf.h"

using namespace constants;

namespace brdf {

InverseSteradians lambertian(const Direction3 & Wi, const Direction3 & Wo, const Direction3 & N)
{
    return 1.0f / PI;
}

InverseSteradians phong(const Direction3 & Wi,
                        const Direction3 & Wo,
                        const Direction3 & N,
                        float a)
{
    // TODO[DAC]: References show both of these as possible normalizing factors.
    //            Both seem to conserve energy. Not sure which is best.
    float normFactor = (a + 1.0) / TWO_PI;
    //float normFactor = (a + 2.0) / TWO_PI;
    return normFactor * std::pow(clampedDot(mirror(Wi, N), Wo), a);
}

brdfSample samplePhong(const vec2 & e,
                       const Direction3 & Wi,
                       const Direction3 & N,
                       const float a)
{
    Direction3 mirrorDir = mirror(Wi, N);
    Direction3 tangent, bitangent;
    coordinate::coordinateSystem(mirrorDir, tangent, bitangent);

    float theta = std::acos(std::pow(e.x, 1.0f / (a + 1.0f)));
    float phi = e.y * constants::TWO_PI;
    float sin_theta = std::sin(theta);

    Direction3 Wo = std::cos(theta) * mirrorDir +
        sin_theta * std::cos(phi) * tangent +
        sin_theta * std::sin(phi) * bitangent;

    float F = phong(Wi, Wo, N, a);

    brdfSample S;
    S.Wo = Wo;
    // PDF includes the projected solid angle factor
    // to ensure energy conservation (cancels out with
    // the dot product in the rendering equation)
    S.pdf = F * clampedDot(Wi, N);;

    return S;
}

}; // brdf

