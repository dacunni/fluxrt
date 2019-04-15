#include <gtest/gtest.h>
#include <cmath>
#include "integrate.h"

namespace {

// Integral of 1 over domain
//
TEST(IntegrateTest, IntegrateOneOverUnitHemisphere_IsTwoPi) {
    const unsigned int thetaSteps = 32, phiSteps = 64;
    float I = integrate::overUnitHemisphere([](float theta, float phi) { return 1.0f; }, thetaSteps, phiSteps);
    EXPECT_NEAR(I, float(2.0 * M_PI), 0.01f);
}

TEST(IntegrateTest, IntegrateOneOverUnitSphere_IsFourPi) {
    const unsigned int thetaSteps = 64, phiSteps = 64;
    float I = integrate::overUnitSphere([](float theta, float phi) { return 1.0f; }, thetaSteps, phiSteps);
    EXPECT_NEAR(I, float(4.0 * M_PI), 0.01f);
}

// Integral of positive constant over domain
//
TEST(IntegrateTest, IntegrateScalerOverUnitHemisphere_IsScalerTimesTwoPi) {
    const unsigned int thetaSteps = 32, phiSteps = 64;
    const float value = 7.0f;
    float I = integrate::overUnitHemisphere([value](float theta, float phi) { return value; }, thetaSteps, phiSteps);
    EXPECT_NEAR(I, float(value * 2.0 * M_PI), 0.01f);
}

TEST(IntegrateTest, IntegrateScalarOverUnitSphere_IsScalarTimesFourPi) {
    const unsigned int thetaSteps = 64, phiSteps = 64;
    const float value = 7.0f;
    float I = integrate::overUnitSphere([value](float theta, float phi) { return value; }, thetaSteps, phiSteps);
    EXPECT_NEAR(I, float(value * 4.0 * M_PI), 0.01f);
}

// Integral of negative constant over domain
//
TEST(IntegrateTest, IntegrateNegativeScalerOverUnitHemisphere_IsScalerTimesTwoPi) {
    const unsigned int thetaSteps = 32, phiSteps = 64;
    const float value = -7.0f;
    float I = integrate::overUnitHemisphere([value](float theta, float phi) { return value; }, thetaSteps, phiSteps);
    EXPECT_NEAR(I, float(value * 2.0 * M_PI), 0.01f);
}

TEST(IntegrateTest, IntegrateNegativeScalarOverUnitSphere_IsScalarTimesFourPi) {
    const unsigned int thetaSteps = 64, phiSteps = 64;
    const float value = -7.0f;
    float I = integrate::overUnitSphere([value](float theta, float phi) { return value; }, thetaSteps, phiSteps);
    EXPECT_NEAR(I, float(value * 4.0 * M_PI), 0.01f);
}

// Integral of cos(theta) over domain
//
TEST(IntegrateTest, IntegrateCosineThetaOverUnitHemisphere_IsPi) {
    const unsigned int thetaSteps = 32, phiSteps = 64;
    float I = integrate::overUnitHemisphere([](float theta, float phi) { return std::cos(theta); }, thetaSteps, phiSteps);
    EXPECT_NEAR(I, float(M_PI), 0.01f);
}

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

