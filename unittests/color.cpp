#include <gtest/gtest.h>
#include <limits>
#include "color.h"

namespace {

// Test minValue<T>()
TEST(ColorChannel, MinValue_Uint8) {
    uint8_t result = color::channel::minValue<uint8_t>();
    EXPECT_EQ(result, 0);
}


TEST(ColorChannel, MinValue_Float) {
    float result = color::channel::minValue<float>();
    EXPECT_EQ(result, 0.0f);
}

TEST(ColorChannel, MinValue_Double) {
    double result = color::channel::minValue<double>();
    EXPECT_EQ(result, 0.0);
}

// Test maxValue<T>()
TEST(ColorChannel, MaxValue_Uint8) {
    uint8_t result = color::channel::maxValue<uint8_t>();
    EXPECT_EQ(result, 255);
}


TEST(ColorChannel, MaxValue_Float) {
    float result = color::channel::maxValue<float>();
    EXPECT_EQ(result, 1.0f);
}

TEST(ColorChannel, MaxValue_Double) {
    double result = color::channel::maxValue<double>();
    EXPECT_EQ(result, 1.0);
}

// Test midValue<T>()
TEST(ColorChannel, MidValue_Uint8) {
    uint8_t result = color::channel::midValue<uint8_t>();
    EXPECT_EQ(result, 127); // 255 / 2 = 127 (integer division)
}

TEST(ColorChannel, MidValue_Float) {
    float result = color::channel::midValue<float>();
    EXPECT_EQ(result, 0.5f);
}

TEST(ColorChannel, MidValue_Double) {
    double result = color::channel::midValue<double>();
    EXPECT_EQ(result, 0.5);
}

// Test clamp<T>() - uint8_t
TEST(ColorChannel, Clamp_Uint8_WithinRange) {
    EXPECT_EQ(color::channel::clamp<uint8_t>(128), 128);
    EXPECT_EQ(color::channel::clamp<uint8_t>(0), 0);
    EXPECT_EQ(color::channel::clamp<uint8_t>(255), 255);
    EXPECT_EQ(color::channel::clamp<uint8_t>(50), 50);
    EXPECT_EQ(color::channel::clamp<uint8_t>(200), 200);
}

TEST(ColorChannel, Clamp_Uint8_BelowMin) {
    // uint8_t has unsigned range, so test with value at min
    EXPECT_EQ(color::channel::clamp<uint8_t>(0), 0);
}

TEST(ColorChannel, Clamp_Uint8_AboveMax) {
    // For uint8_t, we need to test with actual uint8_t values in range
    // Values above 255 wrap due to uint8_t overflow before clamp is called,
    // so we test that values at the boundary are handled correctly
    uint8_t maxVal = 255;
    EXPECT_EQ(color::channel::clamp<uint8_t>(maxVal), 255);
}

// Test clamp<T>() - float
TEST(ColorChannel, Clamp_Float_WithinRange) {
    EXPECT_FLOAT_EQ(color::channel::clamp<float>(0.5f), 0.5f);
    EXPECT_FLOAT_EQ(color::channel::clamp<float>(0.0f), 0.0f);
    EXPECT_FLOAT_EQ(color::channel::clamp<float>(1.0f), 1.0f);
    EXPECT_FLOAT_EQ(color::channel::clamp<float>(0.25f), 0.25f);
    EXPECT_FLOAT_EQ(color::channel::clamp<float>(0.99f), 0.99f);
}

TEST(ColorChannel, Clamp_Float_BelowMin) {
    EXPECT_FLOAT_EQ(color::channel::clamp<float>(-0.5f), 0.0f);
    EXPECT_FLOAT_EQ(color::channel::clamp<float>(-1.0f), 0.0f);
    EXPECT_FLOAT_EQ(color::channel::clamp<float>(-100.5f), 0.0f);
    EXPECT_FLOAT_EQ(color::channel::clamp<float>(-0.0001f), 0.0f);
}

TEST(ColorChannel, Clamp_Float_AboveMax) {
    EXPECT_FLOAT_EQ(color::channel::clamp<float>(1.5f), 1.0f);
    EXPECT_FLOAT_EQ(color::channel::clamp<float>(2.0f), 1.0f);
    EXPECT_FLOAT_EQ(color::channel::clamp<float>(100.5f), 1.0f);
    EXPECT_FLOAT_EQ(color::channel::clamp<float>(1.0001f), 1.0f);
}

// Test clamp<T>() - double
TEST(ColorChannel, Clamp_Double_WithinRange) {
    EXPECT_DOUBLE_EQ(color::channel::clamp<double>(0.5), 0.5);
    EXPECT_DOUBLE_EQ(color::channel::clamp<double>(0.0), 0.0);
    EXPECT_DOUBLE_EQ(color::channel::clamp<double>(1.0), 1.0);
    EXPECT_DOUBLE_EQ(color::channel::clamp<double>(0.25), 0.25);
    EXPECT_DOUBLE_EQ(color::channel::clamp<double>(0.99), 0.99);
}

TEST(ColorChannel, Clamp_Double_BelowMin) {
    EXPECT_DOUBLE_EQ(color::channel::clamp<double>(-0.5), 0.0);
    EXPECT_DOUBLE_EQ(color::channel::clamp<double>(-1.0), 0.0);
    EXPECT_DOUBLE_EQ(color::channel::clamp<double>(-100.5), 0.0);
    EXPECT_DOUBLE_EQ(color::channel::clamp<double>(-0.0001), 0.0);
}

TEST(ColorChannel, Clamp_Double_AboveMax) {
    EXPECT_DOUBLE_EQ(color::channel::clamp<double>(1.5), 1.0);
    EXPECT_DOUBLE_EQ(color::channel::clamp<double>(2.0), 1.0);
    EXPECT_DOUBLE_EQ(color::channel::clamp<double>(100.5), 1.0);
    EXPECT_DOUBLE_EQ(color::channel::clamp<double>(1.0001), 1.0);
}


// Edge cases and stress tests
TEST(ColorChannel, Clamp_Float_VeryLargeValue) {
    EXPECT_FLOAT_EQ(color::channel::clamp<float>(1.0e6f), 1.0f);
    EXPECT_FLOAT_EQ(color::channel::clamp<float>(1.0e10f), 1.0f);
}

TEST(ColorChannel, Clamp_Float_VerySmallNegativeValue) {
    EXPECT_FLOAT_EQ(color::channel::clamp<float>(-1.0e6f), 0.0f);
    EXPECT_FLOAT_EQ(color::channel::clamp<float>(-1.0e10f), 0.0f);
}

TEST(ColorChannel, Clamp_Double_VeryLargeValue) {
    EXPECT_DOUBLE_EQ(color::channel::clamp<double>(1.0e10), 1.0);
    EXPECT_DOUBLE_EQ(color::channel::clamp<double>(1.0e100), 1.0);
}

TEST(ColorChannel, Clamp_Double_VerySmallNegativeValue) {
    EXPECT_DOUBLE_EQ(color::channel::clamp<double>(-1.0e10), 0.0);
    EXPECT_DOUBLE_EQ(color::channel::clamp<double>(-1.0e100), 0.0);
}

// Test that clamp preserves values within epsilon of boundaries
TEST(ColorChannel, Clamp_Float_NearBoundaries) {
    float epsilon = 1.0e-7f;
    EXPECT_FLOAT_EQ(color::channel::clamp<float>(0.0f + epsilon), 0.0f + epsilon);
    EXPECT_FLOAT_EQ(color::channel::clamp<float>(1.0f - epsilon), 1.0f - epsilon);
}

TEST(ColorChannel, Clamp_Double_NearBoundaries) {
    double epsilon = 1.0e-15;
    EXPECT_DOUBLE_EQ(color::channel::clamp<double>(0.0 + epsilon), 0.0 + epsilon);
    EXPECT_DOUBLE_EQ(color::channel::clamp<double>(1.0 - epsilon), 1.0 - epsilon);
}

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
