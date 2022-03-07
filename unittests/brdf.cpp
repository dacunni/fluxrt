#include <gtest/gtest.h>
#include <cmath>
#include "integrate.h"
#include "coordinate.h"
#include "vectortypes.h"
#include "brdf.h"

namespace {

// Utility functions
// TODO: This would be useful as a more general purpose function. Move it somewhere and clean it up.
template<typename F>
void forEachWiWoPairHemisphere(F f, unsigned int thetaSteps, unsigned int phiSteps)
{
    // We're just using integrate::overUnitHemisphere() to walk through theta/phi.
    // Probably should just make a function that does this.
    integrate::overUnitHemisphere(
        [&](float theta1, float phi1) {
            auto wi = Direction3(coordinate::polarToEuclidean(theta1, phi1, 1.0f));
            return integrate::overUnitHemisphere(
                [&](float theta2, float phi2) {
                    auto wo = Direction3(coordinate::polarToEuclidean(theta2, phi2, 1.0f));
                    f(wi, wo);
                    return 0.0f; // dummy
            }, thetaSteps, phiSteps);
        }, thetaSteps, phiSteps);
}

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
                return lambertian(wi, wo, N);
            }, thetaSteps, phiSteps);
    };

    // Vary wo and make sure integral doesn't change
    I = integral(Direction3(0, 0, 1));
    EXPECT_LE(I, 1.0f + closeness);
    I = integral(Direction3(0, 1, 1).normalized());
    EXPECT_LE(I, 1.0f + closeness);
    I = integral(Direction3(-0.2, 0.5, 0.3).normalized());
    EXPECT_LE(I, 1.0f + closeness);
}

TEST(BrdfTest, Integrate_PhongBrdfTimesProjSolidAngle_OverHemisphere_IsOne) {
    const unsigned int thetaSteps = 32, phiSteps = 64;
    Direction3 N(0, 0, 1), wo(0, 0, 1);
    const float closeness = 0.01;
    float I;

    float a = 1.0f;

    auto integral = [N, &a](const Direction3 & wo) {
        return integrate::timesProjectedSolidAngleOverUnitHemisphere(
            [N, wo, &a](float theta, float phi) {
                Direction3 wi = Direction3(coordinate::polarToEuclidean(theta, phi, 1.0f));
                return phong(wi, wo, N, a);
            }, thetaSteps, phiSteps);
    };

    // Vary wo and make sure integral doesn't change
    a = 1.0f;
    I = integral(Direction3(0, 0, 1));
    EXPECT_LE(I, 1.0f + closeness);
    I = integral(Direction3(0, 1, 1).normalized());
    EXPECT_LE(I, 1.0f + closeness);
    I = integral(Direction3(-0.2, 0.5, 0.3).normalized());
    EXPECT_LE(I, 1.0f + closeness);

    a = 10.0f;
    I = integral(Direction3(0, 0, 1));
    EXPECT_LE(I, 1.0f + closeness);
    I = integral(Direction3(0, 1, 1).normalized());
    EXPECT_LE(I, 1.0f + closeness);
    I = integral(Direction3(-0.2, 0.5, 0.3).normalized());
    EXPECT_LE(I, 1.0f + closeness);

    a = 100.0f;
    I = integral(Direction3(0, 0, 1));
    EXPECT_LE(I, 1.0f + closeness);
    I = integral(Direction3(0, 1, 1).normalized());
    EXPECT_LE(I, 1.0f + closeness);
    I = integral(Direction3(-0.2, 0.5, 0.3).normalized());
    EXPECT_LE(I, 1.0f + closeness);
}

//
// Positivity
//

TEST(BrdfTest, Positivity_LambertianBrdf) {
    const unsigned int thetaSteps = 16, phiSteps = 32;
    Direction3 N(0, 0, 1);
    const float closeness = 0.01;

    forEachWiWoPairHemisphere([&](const Direction3 & wi, const Direction3 & wo) {
        EXPECT_TRUE(lambertian(wi, wo, N) >= 0.0f);
    }, thetaSteps, phiSteps);
}

TEST(BrdfTest, Positivity_PhongBrdf) {
    const unsigned int thetaSteps = 16, phiSteps = 32;
    Direction3 N(0, 0, 1);
    const float closeness = 0.01;

    forEachWiWoPairHemisphere([&](const Direction3 & wi, const Direction3 & wo) {
        EXPECT_TRUE(phong(wi, wo, N, 1.0f) >= 0.0f);
    }, thetaSteps, phiSteps);
    forEachWiWoPairHemisphere([&](const Direction3 & wi, const Direction3 & wo) {
        EXPECT_TRUE(phong(wi, wo, N, 10.0f) >= 0.0f);
    }, thetaSteps, phiSteps);
    forEachWiWoPairHemisphere([&](const Direction3 & wi, const Direction3 & wo) {
        EXPECT_TRUE(phong(wi, wo, N, 100.0f) >= 0.0f);
    }, thetaSteps, phiSteps);
}

//
// Helmholtz reciprocity
//

TEST(BrdfTest, HelmholtzReciprocity_LambertianBrdf) {
    const unsigned int thetaSteps = 16, phiSteps = 32;
    Direction3 N(0, 0, 1);
    const float closeness = 0.01;

    forEachWiWoPairHemisphere([&](const Direction3 & wi, const Direction3 & wo) {
        EXPECT_NEAR(phong(wi, wo, N, 1.0f), phong(wo, wi, N, 1.0f), closeness);
    }, thetaSteps, phiSteps);
    forEachWiWoPairHemisphere([&](const Direction3 & wi, const Direction3 & wo) {
        EXPECT_NEAR(phong(wi, wo, N, 10.0f), phong(wo, wi, N, 10.0f), closeness);
    }, thetaSteps, phiSteps);
    forEachWiWoPairHemisphere([&](const Direction3 & wi, const Direction3 & wo) {
        EXPECT_NEAR(phong(wi, wo, N, 100.0f), phong(wo, wi, N, 100.0f), closeness);
    }, thetaSteps, phiSteps);
}

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

