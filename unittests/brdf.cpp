#include <gtest/gtest.h>
#include <cmath>
#include "integrate.h"
#include "coordinate.h"
#include "vectortypes.h"
#include "brdf.h"

namespace {

//
// Energy Conservation
//
TEST(BrdfTest, Integrate_LambertianBrdfTimesProjSolidAngle_OverHemisphere_IsOne) {
    const unsigned int thetaSteps = 32, phiSteps = 64;
    Direction3 N(0, 0, 1), wo(0, 0, 1);
    const float closeness = 0.01;
    float I;

    auto integral = [N](const Direction3 & wo) {
        return integrate::timesProjectedSolidAngleOverUnitHemisphere(
            [N, wo](float theta, float phi) {
                Direction3 wi = Direction3(coordinate::polarToEuclidean(theta, phi, 1.0f));
                return brdf::lambertian(wi, wo, N);
            }, thetaSteps, phiSteps);
    };

    // Vary wo and make sure integral doesn't change
    I = integral(Direction3(0, 0, 1));
    EXPECT_NEAR(I, float(1.0), closeness);
    I = integral(Direction3(0, 1, 1).normalized());
    EXPECT_NEAR(I, float(1.0), closeness);
    I = integral(Direction3(-0.2, 0.5, 0.3).normalized());
    EXPECT_NEAR(I, float(1.0), closeness);
}

// TODO: Helmholtz reciprocity
// TODO: Positivity

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

