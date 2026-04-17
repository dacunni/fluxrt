#include <gtest/gtest.h>
#include <cmath>
#include "interpolation.h"

namespace {

// ============================================================================
// lerp() Tests
// ============================================================================

TEST(LerpTest, LerpAtZero) {
    // At t=0, should return v1
    EXPECT_FLOAT_EQ(lerp(0.0f, 10.0f, 20.0f), 10.0f);
    EXPECT_FLOAT_EQ(lerp(0.0f, -5.0f, 5.0f), -5.0f);
    EXPECT_FLOAT_EQ(lerp(0.0f, 0.0f, 100.0f), 0.0f);
    EXPECT_FLOAT_EQ(lerp(0.0f, 1000.0f, -1000.0f), 1000.0f);
}

TEST(LerpTest, LerpAtOne) {
    // At t=1, should return v2
    EXPECT_FLOAT_EQ(lerp(1.0f, 10.0f, 20.0f), 20.0f);
    EXPECT_FLOAT_EQ(lerp(1.0f, -5.0f, 5.0f), 5.0f);
    EXPECT_FLOAT_EQ(lerp(1.0f, 0.0f, 100.0f), 100.0f);
    EXPECT_FLOAT_EQ(lerp(1.0f, 1000.0f, -1000.0f), -1000.0f);
}

TEST(LerpTest, LerpAtMidpoint) {
    // At t=0.5, should return (v1 + v2) / 2
    EXPECT_FLOAT_EQ(lerp(0.5f, 10.0f, 20.0f), 15.0f);
    EXPECT_FLOAT_EQ(lerp(0.5f, 0.0f, 10.0f), 5.0f);
    EXPECT_FLOAT_EQ(lerp(0.5f, -10.0f, 10.0f), 0.0f);
    EXPECT_FLOAT_EQ(lerp(0.5f, 100.0f, 200.0f), 150.0f);
}

TEST(LerpTest, LerpAtQuarters) {
    // At t=0.25, should return 0.75*v1 + 0.25*v2
    EXPECT_FLOAT_EQ(lerp(0.25f, 0.0f, 100.0f), 25.0f);
    EXPECT_FLOAT_EQ(lerp(0.25f, 100.0f, 200.0f), 125.0f);

    // At t=0.75, should return 0.25*v1 + 0.75*v2
    EXPECT_FLOAT_EQ(lerp(0.75f, 0.0f, 100.0f), 75.0f);
    EXPECT_FLOAT_EQ(lerp(0.75f, 100.0f, 200.0f), 175.0f);
}

TEST(LerpTest, LerpWithNegativeValues) {
    // Test with negative v1 and v2
    EXPECT_FLOAT_EQ(lerp(0.0f, -20.0f, -10.0f), -20.0f);
    EXPECT_FLOAT_EQ(lerp(1.0f, -20.0f, -10.0f), -10.0f);
    EXPECT_FLOAT_EQ(lerp(0.5f, -20.0f, -10.0f), -15.0f);
    EXPECT_FLOAT_EQ(lerp(0.5f, -100.0f, 100.0f), 0.0f);
}

TEST(LerpTest, LerpWithZeroLength) {
    // When v1 == v2, result should always be v1 (== v2)
    EXPECT_FLOAT_EQ(lerp(0.0f, 5.0f, 5.0f), 5.0f);
    EXPECT_FLOAT_EQ(lerp(0.5f, 5.0f, 5.0f), 5.0f);
    EXPECT_FLOAT_EQ(lerp(1.0f, 5.0f, 5.0f), 5.0f);
    EXPECT_FLOAT_EQ(lerp(0.3f, -10.0f, -10.0f), -10.0f);
}

TEST(LerpTest, LerpOutOfBoundsT) {
    // Test with t > 1 (extrapolation beyond v2)
    EXPECT_FLOAT_EQ(lerp(1.5f, 0.0f, 10.0f), 15.0f);
    EXPECT_FLOAT_EQ(lerp(2.0f, 10.0f, 20.0f), 30.0f);

    // Test with t < 0 (extrapolation before v1)
    EXPECT_FLOAT_EQ(lerp(-0.5f, 0.0f, 10.0f), -5.0f);
    EXPECT_FLOAT_EQ(lerp(-1.0f, 10.0f, 20.0f), 0.0f);
}

TEST(LerpTest, LerpWithDoubleT) {
    // Test with double precision t parameter
    EXPECT_DOUBLE_EQ(lerp(0.0, 10.0, 20.0), 10.0);
    EXPECT_DOUBLE_EQ(lerp(1.0, 10.0, 20.0), 20.0);
    EXPECT_DOUBLE_EQ(lerp(0.5, 10.0, 20.0), 15.0);
    EXPECT_DOUBLE_EQ(lerp(0.25, 0.0, 100.0), 25.0);
}


TEST(LerpTest, LerpVerySmallT) {
    // Test with very small t values
    EXPECT_NEAR(lerp(0.001f, 0.0f, 1000.0f), 1.0f, 0.1f);
    EXPECT_NEAR(lerp(0.0001f, 0.0f, 10000.0f), 1.0f, 0.1f);
}

TEST(LerpTest, LerpVeryLargeValues) {
    // Test with very large values
    EXPECT_NEAR(lerp(0.5f, 1e6f, 2e6f), 1.5e6f, 1e3f);
    EXPECT_NEAR(lerp(0.5f, -1e6f, 1e6f), 0.0f, 1e3f);
}

// ============================================================================
// bilerp() Tests
// ============================================================================

TEST(BilerpTest, BilerpAtCorners) {
    // At corners (a=0 or 1, b=0 or 1), should return the corner value
    float v11 = 10.0f, v12 = 20.0f, v21 = 30.0f, v22 = 40.0f;

    // Corner at (0, 0) -> v11
    EXPECT_FLOAT_EQ(bilerp(0.0f, 0.0f, v11, v12, v21, v22), v11);

    // Corner at (1, 0) -> v21
    EXPECT_FLOAT_EQ(bilerp(1.0f, 0.0f, v11, v12, v21, v22), v21);

    // Corner at (0, 1) -> v12
    EXPECT_FLOAT_EQ(bilerp(0.0f, 1.0f, v11, v12, v21, v22), v12);

    // Corner at (1, 1) -> v22
    EXPECT_FLOAT_EQ(bilerp(1.0f, 1.0f, v11, v12, v21, v22), v22);
}

TEST(BilerpTest, BilerpAtCenter) {
    // At center (a=0.5, b=0.5), should return average of all four corners
    float v11 = 0.0f, v12 = 100.0f, v21 = 100.0f, v22 = 200.0f;
    float expected = (v11 + v12 + v21 + v22) / 4.0f; // 100.0f
    EXPECT_FLOAT_EQ(bilerp(0.5f, 0.5f, v11, v12, v21, v22), expected);
}

TEST(BilerpTest, BilerpAlongEdges) {
    // Along bottom edge (b=0)
    float v11 = 0.0f, v12 = 0.0f, v21 = 100.0f, v22 = 100.0f;
    EXPECT_FLOAT_EQ(bilerp(0.5f, 0.0f, v11, v12, v21, v22), 50.0f);

    // Along top edge (b=1)
    EXPECT_FLOAT_EQ(bilerp(0.5f, 1.0f, v11, v12, v21, v22), 50.0f);

    // Along left edge (a=0)
    EXPECT_FLOAT_EQ(bilerp(0.0f, 0.5f, v11, v12, v21, v22), 0.0f);

    // Along right edge (a=1)
    EXPECT_FLOAT_EQ(bilerp(1.0f, 0.5f, v11, v12, v21, v22), 100.0f);
}

TEST(BilerpTest, BilerpQuadrants) {
    // Test sampling in different quadrants
    float v11 = 0.0f, v12 = 0.0f, v21 = 0.0f, v22 = 100.0f;

    // Bottom-left quadrant
    EXPECT_FLOAT_EQ(bilerp(0.25f, 0.25f, v11, v12, v21, v22), 6.25f);

    // Bottom-right quadrant
    EXPECT_FLOAT_EQ(bilerp(0.75f, 0.25f, v11, v12, v21, v22), 18.75f);

    // Top-left quadrant
    EXPECT_FLOAT_EQ(bilerp(0.25f, 0.75f, v11, v12, v21, v22), 18.75f);

    // Top-right quadrant
    EXPECT_FLOAT_EQ(bilerp(0.75f, 0.75f, v11, v12, v21, v22), 56.25f);
}

TEST(BilerpTest, BilerpWithNegativeValues) {
    float v11 = -10.0f, v12 = -20.0f, v21 = -30.0f, v22 = -40.0f;

    EXPECT_FLOAT_EQ(bilerp(0.0f, 0.0f, v11, v12, v21, v22), v11);
    EXPECT_FLOAT_EQ(bilerp(1.0f, 1.0f, v11, v12, v21, v22), v22);
    EXPECT_FLOAT_EQ(bilerp(0.5f, 0.5f, v11, v12, v21, v22), -25.0f);
}

TEST(BilerpTest, BilerpOutOfBounds) {
    // Test with parameters outside [0, 1]
    float v11 = 0.0f, v12 = 100.0f, v21 = 100.0f, v22 = 200.0f;

    // Extrapolation beyond 1
    float result_high = bilerp(1.5f, 1.5f, v11, v12, v21, v22);
    EXPECT_GT(result_high, v22); // Should be beyond the max corner

    // Extrapolation before 0
    float result_low = bilerp(-0.5f, -0.5f, v11, v12, v21, v22);
    EXPECT_LT(result_low, v11); // Should be before the min corner
}

TEST(BilerpTest, BilerpWithUniformValues) {
    // When all corners are the same, result should always be that value
    float v = 42.0f;
    EXPECT_FLOAT_EQ(bilerp(0.0f, 0.0f, v, v, v, v), v);
    EXPECT_FLOAT_EQ(bilerp(0.5f, 0.5f, v, v, v, v), v);
    EXPECT_FLOAT_EQ(bilerp(1.0f, 1.0f, v, v, v, v), v);
    EXPECT_FLOAT_EQ(bilerp(0.3f, 0.7f, v, v, v, v), v);
}

// ============================================================================
// lerpFromTo() Tests
// ============================================================================

TEST(LerpFromToTest, BasicRangeMapping) {
    // Map from range [0, 10] to range [0, 100]
    EXPECT_FLOAT_EQ(lerpFromTo(0.0f, 0.0f, 10.0f, 0.0f, 100.0f), 0.0f);
    EXPECT_FLOAT_EQ(lerpFromTo(5.0f, 0.0f, 10.0f, 0.0f, 100.0f), 50.0f);
    EXPECT_FLOAT_EQ(lerpFromTo(10.0f, 0.0f, 10.0f, 0.0f, 100.0f), 100.0f);
}

TEST(LerpFromToTest, RangeMappingWithNegatives) {
    // Map from range [-10, 10] to range [0, 100]
    EXPECT_FLOAT_EQ(lerpFromTo(-10.0f, -10.0f, 10.0f, 0.0f, 100.0f), 0.0f);
    EXPECT_FLOAT_EQ(lerpFromTo(0.0f, -10.0f, 10.0f, 0.0f, 100.0f), 50.0f);
    EXPECT_FLOAT_EQ(lerpFromTo(10.0f, -10.0f, 10.0f, 0.0f, 100.0f), 100.0f);
}

TEST(LerpFromToTest, RangeMappingToNegative) {
    // Map from range [0, 10] to range [-50, 50]
    EXPECT_FLOAT_EQ(lerpFromTo(0.0f, 0.0f, 10.0f, -50.0f, 50.0f), -50.0f);
    EXPECT_FLOAT_EQ(lerpFromTo(5.0f, 0.0f, 10.0f, -50.0f, 50.0f), 0.0f);
    EXPECT_FLOAT_EQ(lerpFromTo(10.0f, 0.0f, 10.0f, -50.0f, 50.0f), 50.0f);
}

TEST(LerpFromToTest, RangeMappingBothNegative) {
    // Map from range [-100, -10] to range [-50, -5]
    EXPECT_FLOAT_EQ(lerpFromTo(-100.0f, -100.0f, -10.0f, -50.0f, -5.0f), -50.0f);
    EXPECT_FLOAT_EQ(lerpFromTo(-55.0f, -100.0f, -10.0f, -50.0f, -5.0f), -27.5f);
    EXPECT_FLOAT_EQ(lerpFromTo(-10.0f, -100.0f, -10.0f, -50.0f, -5.0f), -5.0f);
}

TEST(LerpFromToTest, ZeroWidthRangeFromMin) {
    // When fromMin == fromMax, should return toMin
    EXPECT_FLOAT_EQ(lerpFromTo(5.0f, 10.0f, 10.0f, 0.0f, 100.0f), 0.0f);
    EXPECT_FLOAT_EQ(lerpFromTo(10.0f, 10.0f, 10.0f, -50.0f, 50.0f), -50.0f);
    EXPECT_FLOAT_EQ(lerpFromTo(100.0f, 50.0f, 50.0f, 25.0f, 75.0f), 25.0f);
}

TEST(LerpFromToTest, OutOfBoundsInput) {
    // Map value outside the source range
    // Value below source range
    EXPECT_FLOAT_EQ(lerpFromTo(-5.0f, 0.0f, 10.0f, 0.0f, 100.0f), -50.0f);

    // Value above source range
    EXPECT_FLOAT_EQ(lerpFromTo(15.0f, 0.0f, 10.0f, 0.0f, 100.0f), 150.0f);
}

TEST(LerpFromToTest, ReversedSourceRange) {
    // fromMin > fromMax (reversed range)
    // When source range is reversed, behavior inverts: fromMin maps to toMin, fromMax maps to toMax
    EXPECT_FLOAT_EQ(lerpFromTo(10.0f, 10.0f, 0.0f, 0.0f, 100.0f), 0.0f);
    EXPECT_FLOAT_EQ(lerpFromTo(5.0f, 10.0f, 0.0f, 0.0f, 100.0f), 50.0f);
    EXPECT_FLOAT_EQ(lerpFromTo(0.0f, 10.0f, 0.0f, 0.0f, 100.0f), 100.0f);
}

TEST(LerpFromToTest, ReversedTargetRange) {
    // toMin > toMax (reversed target range)
    EXPECT_FLOAT_EQ(lerpFromTo(0.0f, 0.0f, 10.0f, 100.0f, 0.0f), 100.0f);
    EXPECT_FLOAT_EQ(lerpFromTo(5.0f, 0.0f, 10.0f, 100.0f, 0.0f), 50.0f);
    EXPECT_FLOAT_EQ(lerpFromTo(10.0f, 0.0f, 10.0f, 100.0f, 0.0f), 0.0f);
}

TEST(LerpFromToTest, SmallRanges) {
    // Test with very small ranges
    EXPECT_NEAR(lerpFromTo(0.5f, 0.0f, 1.0f, 0.0f, 1.0f), 0.5f, 1e-6f);
    EXPECT_NEAR(lerpFromTo(0.25f, 0.0f, 1.0f, 0.0f, 1.0f), 0.25f, 1e-6f);
}

TEST(LerpFromToTest, LargeRanges) {
    // Test with very large ranges
    EXPECT_NEAR(lerpFromTo(50000.0f, 0.0f, 100000.0f, 0.0f, 1000000.0f),
                500000.0f, 1.0f);
}

TEST(LerpFromToTest, IdentityMapping) {
    // Map from [0, 10] to [0, 10] - should be identity
    EXPECT_FLOAT_EQ(lerpFromTo(0.0f, 0.0f, 10.0f, 0.0f, 10.0f), 0.0f);
    EXPECT_FLOAT_EQ(lerpFromTo(5.0f, 0.0f, 10.0f, 0.0f, 10.0f), 5.0f);
    EXPECT_FLOAT_EQ(lerpFromTo(10.0f, 0.0f, 10.0f, 0.0f, 10.0f), 10.0f);
}

TEST(LerpFromToTest, NegationMapping) {
    // Map from [0, 10] to [10, 0] - should negate
    EXPECT_FLOAT_EQ(lerpFromTo(0.0f, 0.0f, 10.0f, 10.0f, 0.0f), 10.0f);
    EXPECT_FLOAT_EQ(lerpFromTo(5.0f, 0.0f, 10.0f, 10.0f, 0.0f), 5.0f);
    EXPECT_FLOAT_EQ(lerpFromTo(10.0f, 0.0f, 10.0f, 10.0f, 0.0f), 0.0f);
}

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
