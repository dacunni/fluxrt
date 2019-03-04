#include <gtest/gtest.h>
#include "micromath.h"

namespace {

TEST(BarycentricInitTest, DefaultConstruct) {
    BarycentricCoordinate bary;
    EXPECT_FLOAT_EQ(bary.u, 0.0f);
    EXPECT_FLOAT_EQ(bary.v, 0.0f);
    EXPECT_FLOAT_EQ(bary.w, 1.0f);
}

#define EXPECT_VEC3_EQ(a, b) \
    EXPECT_FLOAT_EQ(a.x, b.x); \
    EXPECT_FLOAT_EQ(a.y, b.y); \
    EXPECT_FLOAT_EQ(a.z, b.z);

TEST(BarycentricEvalExtremeTest, UVertex) {
    BarycentricCoordinate bary(1.0f, 0.0f, 0.0f);
    Position3 v0(1.0f, 4.2f, 2.4f);
    Position3 v1(5.8f, 5.2f, 1.4f);
    Position3 v2(3.5f, -4.5f, -3.2f);

    auto interp = interpolate(v0, v1, v2, bary);
    EXPECT_VEC3_EQ(v0, interp);
}

TEST(BarycentricEvalExtremeTest, VVertex) {
    BarycentricCoordinate bary(0.0f, 1.0f, 0.0f);
    Position3 v0(1.0f, 4.2f, 2.4f);
    Position3 v1(5.8f, 5.2f, 1.4f);
    Position3 v2(3.5f, -4.5f, -3.2f);

    auto interp = interpolate(v0, v1, v2, bary);
    EXPECT_VEC3_EQ(v1, interp);
}

TEST(BarycentricEvalExtremeTest, WVertex) {
    BarycentricCoordinate bary(0.0f, 0.0f, 1.0f);
    Position3 v0(1.0f, 4.2f, 2.4f);
    Position3 v1(5.8f, 5.2f, 1.4f);
    Position3 v2(3.5f, -4.5f, -3.2f);

    auto interp = interpolate(v0, v1, v2, bary);
    EXPECT_VEC3_EQ(v2, interp);
}

// Test that equally weighted barycentric coodinates give the
// center of mass for an equilateral triangle
TEST(BarycentricEvalTest, EquilateralCenterOfMass) {
    BarycentricCoordinate bary(1.0f/3.0f, 1.0f/3.0f, 1.0f/3.0f);

    Position3 v0(0.0f, 0.0f, 4.0f);
    Position3 v1(-4.0f, 0.0f, 0.0f);
    Position3 v2(4.0f, 0.0f, 0.0f);
    Position3 com = Position3((v0 + v1 + v2) / 3.0f);

    auto interp = interpolate(v0, v1, v2, bary);
    EXPECT_VEC3_EQ(com, interp);
}

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

