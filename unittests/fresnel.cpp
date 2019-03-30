#include <gtest/gtest.h>
#include <cmath>
#include "optics.h"
#include "fresnel.h"

namespace {

TEST(FresnelBoundaryTest, Vaccuum_To_Dialectric_Normal) {
    // Same IOR
    EXPECT_FLOAT_EQ(0.00f, fresnel::dialectric::normal(1.0f, 1.0f));
    // Different IOR
    EXPECT_FLOAT_EQ(0.04f, fresnel::dialectric::normal(1.0f, 1.5f));
}

TEST(FresnelBoundaryTest, Vaccuum_To_Dialectric_Unpolarized) {
    // Same IOR
    EXPECT_FLOAT_EQ(0.00f, fresnel::dialectric::unpolarizedSnell(std::cos(0.0f), 1.0f, 1.0f));
    EXPECT_FLOAT_EQ(1.00f, fresnel::dialectric::unpolarizedSnell(std::cos(M_PI * 0.5f), 1.0f, 1.0f));
    // Different IOR
    EXPECT_FLOAT_EQ(0.04f, fresnel::dialectric::unpolarizedSnell(std::cos(0.0f), 1.0f, 1.5f));
    EXPECT_FLOAT_EQ(1.00f, fresnel::dialectric::unpolarizedSnell(std::cos(M_PI * 0.5f), 1.0f, 1.5f));
}

TEST(FresnelBoundaryTest, Vaccuum_To_Dialectric_Parallel) {
    // Same IOR
    EXPECT_FLOAT_EQ(0.00f, fresnel::dialectric::parallelSnell(std::cos(0.0f), 1.0f, 1.0f));
    EXPECT_FLOAT_EQ(1.00f, fresnel::dialectric::parallelSnell(std::cos(M_PI * 0.5f), 1.0f, 1.0f));
    // Different IOR
    EXPECT_FLOAT_EQ(0.04f, fresnel::dialectric::parallelSnell(std::cos(0.0f), 1.0f, 1.5f));
    EXPECT_FLOAT_EQ(1.00f, fresnel::dialectric::parallelSnell(std::cos(M_PI * 0.5f), 1.0f, 1.5f));
}

TEST(FresnelBoundaryTest, Vaccuum_To_Dialectric_Perpendicular) {
    // Same IOR
    EXPECT_FLOAT_EQ(0.00f, fresnel::dialectric::perpendicularSnell(std::cos(0.0f), 1.0f, 1.0f));
    EXPECT_FLOAT_EQ(1.00f, fresnel::dialectric::perpendicularSnell(std::cos(M_PI * 0.5f), 1.0f, 1.0f));
    // Different IOR
    EXPECT_FLOAT_EQ(0.04f, fresnel::dialectric::perpendicularSnell(std::cos(0.0f), 1.0f, 1.5f));
    EXPECT_FLOAT_EQ(1.00f, fresnel::dialectric::perpendicularSnell(std::cos(M_PI * 0.5f), 1.0f, 1.5f));
}

TEST(FresnelBoundaryTest, Vaccuum_To_Dialectric_Schlick) {
    float F0, nt;
    // Same IOR
    nt = 1.0f;
    F0 = fresnel::dialectric::normal(1.0f, nt);
    EXPECT_FLOAT_EQ(fresnel::dialectric::normal(1.0f, nt), fresnel::schlick(F0, std::cos(0.0f)));
    EXPECT_FLOAT_EQ(1.00f, fresnel::schlick(F0, std::cos(M_PI * 0.5f)));
    // Different IOR
    nt = 1.5f;
    F0 = fresnel::dialectric::normal(1.0f, nt);
    EXPECT_FLOAT_EQ(fresnel::dialectric::normal(1.0f, 1.5f), fresnel::schlick(F0, std::cos(0.0f)));
    EXPECT_FLOAT_EQ(1.00f, fresnel::schlick(F0, std::cos(M_PI * 0.5f)));
    nt = 2.0f;
    F0 = fresnel::dialectric::normal(1.0f, nt);
    EXPECT_FLOAT_EQ(fresnel::dialectric::normal(1.0f, nt), fresnel::schlick(F0, std::cos(0.0f)));
    EXPECT_FLOAT_EQ(1.00f, fresnel::schlick(F0, std::cos(M_PI * 0.5f)));
}

TEST(FresnelBoundaryTest, Dialectric_To_Vaccuum_Snell_TotalInternalReflection) {
    float delta = 0.05; // fudge factor around critical angle
    float criticalAngle = M_PI * 41.0f / 180.0f; // ~41 deg for ni 1.5

    // NaNs should only happen above the critical angle calculating Snell transmission angle
    EXPECT_TRUE(!std::isnan(optics::snellsLawAngle(1.5f, criticalAngle - delta, 1.0f)));
    EXPECT_TRUE(std::isnan(optics::snellsLawAngle(1.5f, criticalAngle + delta, 1.0f)));
}

TEST(FresnelBoundaryTest, Dialectric_To_Vaccuum_Unpolarized_TotalInternalReflection) {
    float delta = 0.05; // fudge factor around critical angle
    float criticalAngle = M_PI * 41.0f / 180.0f; // ~41 deg for ni 1.5

    // Reflectance should be 1 above critical angle
    EXPECT_GT(1.0f, fresnel::dialectric::unpolarizedSnell(std::cos(criticalAngle - delta), 1.5f, 1.0f));
    EXPECT_FLOAT_EQ(1.0f, fresnel::dialectric::unpolarizedSnell(std::cos(criticalAngle + delta), 1.5f, 1.0f));
    EXPECT_FLOAT_EQ(1.0f, fresnel::dialectric::unpolarizedSnell(std::cos(M_PI * 0.5), 1.5f, 1.0f));
}

TEST(FresnelBoundaryTest, Dialectric_To_Vaccuum_Parallel_TotalInternalReflection) {
    float delta = 0.05; // fudge factor around critical angle
    float criticalAngle = M_PI * 41.0f / 180.0f; // ~41 deg for ni 1.5

    // Reflectance should be 1 above critical angle
    EXPECT_GT(1.0f, fresnel::dialectric::parallelSnell(std::cos(criticalAngle - delta), 1.5f, 1.0f));
    EXPECT_FLOAT_EQ(1.0f, fresnel::dialectric::parallelSnell(std::cos(criticalAngle + delta), 1.5f, 1.0f));
    EXPECT_FLOAT_EQ(1.0f, fresnel::dialectric::parallelSnell(std::cos(M_PI * 0.5), 1.5f, 1.0f));
}

TEST(FresnelBoundaryTest, Dialectric_To_Vaccuum_Perpendicular_TotalInternalReflection) {
    float delta = 0.05; // fudge factor around critical angle
    float criticalAngle = M_PI * 41.0f / 180.0f; // ~41 deg for ni 1.5

    // Reflectance should be 1 above critical angle
    EXPECT_GT(1.0f, fresnel::dialectric::perpendicularSnell(std::cos(criticalAngle - delta), 1.5f, 1.0f));
    EXPECT_FLOAT_EQ(1.0f, fresnel::dialectric::perpendicularSnell(std::cos(criticalAngle + delta), 1.5f, 1.0f));
    EXPECT_FLOAT_EQ(1.0f, fresnel::dialectric::perpendicularSnell(std::cos(M_PI * 0.5), 1.5f, 1.0f));
}

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

