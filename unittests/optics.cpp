#include <gtest/gtest.h>
#include <cmath>
#include "optics.h"

namespace {

// ---- Snell's Law (angle form) ----

TEST(SnellsLawAngle, NormalIncidenceStaysZero) {
    EXPECT_FLOAT_EQ(0.0f, optics::snellsLawAngle(1.0f, 0.0f, 1.5f));
    EXPECT_FLOAT_EQ(0.0f, optics::snellsLawAngle(1.5f, 0.0f, 1.0f));
}

TEST(SnellsLawAngle, AirToGlass45Degrees) {
    // n_i=1.0, angle_i=45deg, n_o=1.5 -> asin(sin(45)/1.5)
    float angle_i = M_PI / 4.0f;
    float expected = asinf(1.0f / 1.5f * sinf(angle_i));
    EXPECT_NEAR(expected, optics::snellsLawAngle(1.0f, angle_i, 1.5f), 1e-6f);
}

TEST(SnellsLawAngle, Symmetry) {
    // air -> glass then glass -> air should recover original angle
    float angle_i = 0.5f; // radians
    float angle_t = optics::snellsLawAngle(1.0f, angle_i, 1.5f);
    float angle_r = optics::snellsLawAngle(1.5f, angle_t, 1.0f);
    EXPECT_NEAR(angle_i, angle_r, 1e-6f);
}

TEST(SnellsLawAngle, SameIORUnchanged) {
    float angle_i = 0.7f;
    EXPECT_NEAR(angle_i, optics::snellsLawAngle(1.5f, angle_i, 1.5f), 1e-6f);
}

TEST(SnellsLawAngle, TotalInternalReflectionIsNaN) {
    // Critical angle for glass (1.5) -> air: asin(1/1.5) ~ 41.8 deg
    // Use an angle beyond that
    float angle_i = 1.0f; // ~57 degrees, past critical angle
    float result = optics::snellsLawAngle(1.5f, angle_i, 1.0f);
    EXPECT_TRUE(std::isnan(result));
}

// ---- Snell's Law (sine form) ----

TEST(SnellsLawSine, ZeroSineStaysZero) {
    EXPECT_FLOAT_EQ(0.0f, optics::snellsLawSine(1.0f, 0.0f, 1.5f));
}

TEST(SnellsLawSine, SameIORUnchanged) {
    EXPECT_NEAR(0.5f, optics::snellsLawSine(1.5f, 0.5f, 1.5f), 1e-6f);
}

TEST(SnellsLawSine, Reciprocity) {
    float sin_i = 0.5f;
    float sin_t = optics::snellsLawSine(1.0f, sin_i, 1.5f);
    float sin_r = optics::snellsLawSine(1.5f, sin_t, 1.0f);
    EXPECT_NEAR(sin_i, sin_r, 1e-6f);
}

TEST(SnellsLawSine, TIRConditionSineExceedsOne) {
    // Dense to rare at steep angle: sine result > 1 indicates TIR
    float result = optics::snellsLawSine(1.5f, 1.0f, 1.0f);
    EXPECT_GT(result, 1.0f);
}

// ---- Beer's Law (scalar) ----

TEST(BeersLawScalar, ZeroAttenuationReturnsOne) {
    EXPECT_FLOAT_EQ(1.0f, optics::beersLawAttenuation(0.0f, 10.0f));
}

TEST(BeersLawScalar, ZeroDistanceReturnsOne) {
    EXPECT_FLOAT_EQ(1.0f, optics::beersLawAttenuation(1.0f, 0.0f));
}

TEST(BeersLawScalar, UnitAttUnitDist) {
    EXPECT_NEAR(1.0f / M_E, optics::beersLawAttenuation(1.0f, 1.0f), 1e-6f);
}

TEST(BeersLawScalar, AdditiveDistances) {
    float att = 0.5f, d1 = 2.0f, d2 = 3.0f;
    float combined = optics::beersLawAttenuation(att, d1 + d2);
    float sequential = optics::beersLawAttenuation(att, d1) * optics::beersLawAttenuation(att, d2);
    EXPECT_NEAR(combined, sequential, 1e-6f);
}

TEST(BeersLawScalar, MonotonicallyDecreasing) {
    float att = 1.0f;
    float prev = optics::beersLawAttenuation(att, 0.0f);
    for(float dist = 0.5f; dist <= 5.0f; dist += 0.5f) {
        float curr = optics::beersLawAttenuation(att, dist);
        EXPECT_LT(curr, prev);
        prev = curr;
    }
}

// ---- Beer's Law (RGB) ----

TEST(BeersLawRGB, ZeroAttenuationReturnsWhite) {
    ParameterRGB att(0.0f, 0.0f, 0.0f);
    auto result = optics::beersLawAttenuation(att, 10.0f);
    EXPECT_NEAR(1.0f, result.r, 1e-6f);
    EXPECT_NEAR(1.0f, result.g, 1e-6f);
    EXPECT_NEAR(1.0f, result.b, 1e-6f);
}

TEST(BeersLawRGB, ChannelsAreIndependent) {
    // Only red attenuates; green and blue should stay at 1.0
    ParameterRGB att(1.0f, 0.0f, 0.0f);
    auto result = optics::beersLawAttenuation(att, 2.0f);
    EXPECT_NEAR(std::exp(-2.0f), result.r, 1e-6f);
    EXPECT_NEAR(1.0f, result.g, 1e-6f);
    EXPECT_NEAR(1.0f, result.b, 1e-6f);
}

// ---- Sellmeier ----

TEST(OpticsSellmeierIor, General3Coeffs) {
    // Glass, BK7 Schott - https://refractiveindex.info/?shelf=glass&book=BK7&page=SCHOTT
    float B[3] = { 1.03961212, 0.231792344, 1.01046945 };
    float C[3] = { 0.00600069867, 0.0200179144, 103.560653 };
    EXPECT_NEAR(1.5392, optics::ior::sellmeier(0.35, B, C, 3), 0.0001);
    EXPECT_NEAR(1.4945, optics::ior::sellmeier(2.0, B, C, 3), 0.0001);
}

TEST(OpticsSellmeierIor, SingleCoeffKnownValue) {
    // n^2 = 1 + B*w^2/(w^2 - C), with B=1, C=0 -> n^2 = 2 -> n = sqrt(2)
    float B[1] = { 1.0f };
    float C[1] = { 0.0f };
    EXPECT_NEAR(std::sqrt(2.0f), optics::ior::sellmeier(1.0f, B, C, 1), 1e-6f);
}

TEST(OpticsSellmeierIor, IORAlwaysAboveOne) {
    float B[3] = { 1.03961212, 0.231792344, 1.01046945 };
    float C[3] = { 0.00600069867, 0.0200179144, 103.560653 };
    for(float wl = 0.4f; wl <= 2.0f; wl += 0.1f) {
        EXPECT_GT(optics::ior::sellmeier(wl, B, C, 3), 1.0f);
    }
}

TEST(OpticsSellmeierIor, NormalDispersionBK7) {
    // BK7 glass has normal dispersion: IOR decreases as wavelength increases
    float B[3] = { 1.03961212, 0.231792344, 1.01046945 };
    float C[3] = { 0.00600069867, 0.0200179144, 103.560653 };
    float wavelengths[] = { 0.4f, 0.55f, 0.7f, 1.0f };
    float prev = optics::ior::sellmeier(wavelengths[0], B, C, 3);
    for(int i = 1; i < 4; ++i) {
        float curr = optics::ior::sellmeier(wavelengths[i], B, C, 3);
        EXPECT_LT(curr, prev);
        prev = curr;
    }
}

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
