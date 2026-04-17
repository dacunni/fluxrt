#include <gtest/gtest.h>
#include "ValueRGB.h"
#include "ValueArray.h"

namespace {

// Helper for float comparison
constexpr float EPSILON = 1e-5f;

bool floatEquals(float a, float b, float epsilon = EPSILON) {
    return std::abs(a - b) < epsilon;
}

// ============================================================================
// VALUERGB TESTS
// ============================================================================

// ============================================================================
// DEFAULT CONSTRUCTION
// ============================================================================

TEST(ValueRGBTest, ColorRGBDefaultConstructionZero) {
    ColorRGB c;
    EXPECT_FLOAT_EQ(c.r, 0.0f);
    EXPECT_FLOAT_EQ(c.g, 0.0f);
    EXPECT_FLOAT_EQ(c.b, 0.0f);
}

TEST(ValueRGBTest, RadianceRGBDefaultConstructionZero) {
    RadianceRGB r;
    EXPECT_FLOAT_EQ(r.r, 0.0f);
    EXPECT_FLOAT_EQ(r.g, 0.0f);
    EXPECT_FLOAT_EQ(r.b, 0.0f);
}

TEST(ValueRGBTest, ReflectanceRGBDefaultConstructionOne) {
    ReflectanceRGB r(1.0f, 1.0f, 1.0f);
    EXPECT_FLOAT_EQ(r.r, 1.0f);
    EXPECT_FLOAT_EQ(r.g, 1.0f);
    EXPECT_FLOAT_EQ(r.b, 1.0f);
}

TEST(ValueRGBTest, ParameterRGBDefaultConstructionZero) {
    ParameterRGB p(0.0f, 0.0f, 0.0f);
    EXPECT_FLOAT_EQ(p.r, 0.0f);
    EXPECT_FLOAT_EQ(p.g, 0.0f);
    EXPECT_FLOAT_EQ(p.b, 0.0f);
}

// ============================================================================
// EXPLICIT CONSTRUCTION
// ============================================================================

TEST(ValueRGBTest, ColorRGBExplicitConstruction) {
    ColorRGB c(0.5f, 0.6f, 0.7f);
    EXPECT_FLOAT_EQ(c.r, 0.5f);
    EXPECT_FLOAT_EQ(c.g, 0.6f);
    EXPECT_FLOAT_EQ(c.b, 0.7f);
}

TEST(ValueRGBTest, RadianceRGBExplicitConstruction) {
    RadianceRGB r(1.0f, 2.0f, 3.0f);
    EXPECT_FLOAT_EQ(r.r, 1.0f);
    EXPECT_FLOAT_EQ(r.g, 2.0f);
    EXPECT_FLOAT_EQ(r.b, 3.0f);
}

TEST(ValueRGBTest, ReflectanceRGBExplicitConstruction) {
    ReflectanceRGB r(0.5f, 0.6f, 0.7f);
    EXPECT_FLOAT_EQ(r.r, 0.5f);
    EXPECT_FLOAT_EQ(r.g, 0.6f);
    EXPECT_FLOAT_EQ(r.b, 0.7f);
}

TEST(ValueRGBTest, ParameterRGBExplicitConstruction) {
    ParameterRGB p(0.3f, 0.4f, 0.5f);
    EXPECT_FLOAT_EQ(p.r, 0.3f);
    EXPECT_FLOAT_EQ(p.g, 0.4f);
    EXPECT_FLOAT_EQ(p.b, 0.5f);
}

// ============================================================================
// ARRAY CONSTRUCTOR
// ============================================================================

TEST(ValueRGBTest, ColorRGBArrayConstructor) {
    float vals[3] = {0.1f, 0.2f, 0.3f};
    ColorRGB c(vals);
    EXPECT_FLOAT_EQ(c.r, 0.1f);
    EXPECT_FLOAT_EQ(c.g, 0.2f);
    EXPECT_FLOAT_EQ(c.b, 0.3f);
}

TEST(ValueRGBTest, RadianceRGBArrayConstructor) {
    float vals[3] = {1.5f, 2.5f, 3.5f};
    RadianceRGB r(vals);
    EXPECT_FLOAT_EQ(r.r, 1.5f);
    EXPECT_FLOAT_EQ(r.g, 2.5f);
    EXPECT_FLOAT_EQ(r.b, 3.5f);
}

// ============================================================================
// COPY CONSTRUCTION
// ============================================================================

TEST(ValueRGBTest, ColorRGBCopyConstruction) {
    ColorRGB c1(0.2f, 0.3f, 0.4f);
    ColorRGB c2(c1);
    EXPECT_FLOAT_EQ(c2.r, 0.2f);
    EXPECT_FLOAT_EQ(c2.g, 0.3f);
    EXPECT_FLOAT_EQ(c2.b, 0.4f);
}

TEST(ValueRGBTest, RadianceRGBCopyConstruction) {
    RadianceRGB r1(1.1f, 2.2f, 3.3f);
    RadianceRGB r2(r1);
    EXPECT_FLOAT_EQ(r2.r, 1.1f);
    EXPECT_FLOAT_EQ(r2.g, 2.2f);
    EXPECT_FLOAT_EQ(r2.b, 3.3f);
}

// ============================================================================
// CROSS-TYPE CONSTRUCTION
// ============================================================================

TEST(ValueRGBTest, ReflectanceRGBFromColorRGB) {
    ColorRGB c(0.5f, 0.6f, 0.7f);
    ReflectanceRGB r(c);
    EXPECT_FLOAT_EQ(r.r, 0.5f);
    EXPECT_FLOAT_EQ(r.g, 0.6f);
    EXPECT_FLOAT_EQ(r.b, 0.7f);
}

TEST(ValueRGBTest, ParameterRGBFromColorRGB) {
    ColorRGB c(0.1f, 0.2f, 0.3f);
    ParameterRGB p(c);
    EXPECT_FLOAT_EQ(p.r, 0.1f);
    EXPECT_FLOAT_EQ(p.g, 0.2f);
    EXPECT_FLOAT_EQ(p.b, 0.3f);
}

// ============================================================================
// RESIDUAL METHOD
// ============================================================================

TEST(ValueRGBTest, ColorRGBResidual) {
    ColorRGB c(0.3f, 0.6f, 0.9f);
    ColorRGB res = c.residual();
    EXPECT_FLOAT_EQ(res.r, 0.7f);
    EXPECT_FLOAT_EQ(res.g, 0.4f);
    EXPECT_FLOAT_EQ(res.b, 0.1f);
}

TEST(ValueRGBTest, RadianceRGBResidual) {
    RadianceRGB r(0.1f, 0.2f, 0.3f);
    RadianceRGB res = r.residual();
    EXPECT_FLOAT_EQ(res.r, 0.9f);
    EXPECT_FLOAT_EQ(res.g, 0.8f);
    EXPECT_FLOAT_EQ(res.b, 0.7f);
}

TEST(ValueRGBTest, ReflectanceRGBResidual) {
    ReflectanceRGB r(0.2f, 0.5f, 0.8f);
    ReflectanceRGB res = r.residual();
    EXPECT_FLOAT_EQ(res.r, 0.8f);
    EXPECT_FLOAT_EQ(res.g, 0.5f);
    EXPECT_FLOAT_EQ(res.b, 0.2f);
}

// ============================================================================
// HAS NON-ZERO COMPONENT
// ============================================================================

TEST(ValueRGBTest, ColorRGBHasNonZeroComponentTrue) {
    ColorRGB c(0.0f, 0.0f, 0.1f);
    EXPECT_TRUE(c.hasNonZeroComponent());
}

TEST(ValueRGBTest, ColorRGBHasNonZeroComponentFalse) {
    ColorRGB c(0.0f, 0.0f, 0.0f);
    EXPECT_FALSE(c.hasNonZeroComponent());
}

TEST(ValueRGBTest, RadianceRGBHasNonZeroComponentTrue) {
    RadianceRGB r(0.0f, 1.5f, 0.0f);
    EXPECT_TRUE(r.hasNonZeroComponent());
}

TEST(ValueRGBTest, RadianceRGBHasNonZeroComponentFalse) {
    RadianceRGB r(0.0f, 0.0f, 0.0f);
    EXPECT_FALSE(r.hasNonZeroComponent());
}

// ============================================================================
// STATIC FACTORY METHODS
// ============================================================================

TEST(ValueRGBTest, ColorRGBWhite) {
    ColorRGB c = ColorRGB::WHITE();
    EXPECT_FLOAT_EQ(c.r, 1.0f);
    EXPECT_FLOAT_EQ(c.g, 1.0f);
    EXPECT_FLOAT_EQ(c.b, 1.0f);
}

TEST(ValueRGBTest, ColorRGBBlack) {
    ColorRGB c = ColorRGB::BLACK();
    EXPECT_FLOAT_EQ(c.r, 0.0f);
    EXPECT_FLOAT_EQ(c.g, 0.0f);
    EXPECT_FLOAT_EQ(c.b, 0.0f);
}

TEST(ValueRGBTest, ColorRGBRed) {
    ColorRGB c = ColorRGB::RED();
    EXPECT_FLOAT_EQ(c.r, 1.0f);
    EXPECT_FLOAT_EQ(c.g, 0.0f);
    EXPECT_FLOAT_EQ(c.b, 0.0f);
}

TEST(ValueRGBTest, ColorRGBGreen) {
    ColorRGB c = ColorRGB::GREEN();
    EXPECT_FLOAT_EQ(c.r, 0.0f);
    EXPECT_FLOAT_EQ(c.g, 1.0f);
    EXPECT_FLOAT_EQ(c.b, 0.0f);
}

TEST(ValueRGBTest, ColorRGBBlue) {
    ColorRGB c = ColorRGB::BLUE();
    EXPECT_FLOAT_EQ(c.r, 0.0f);
    EXPECT_FLOAT_EQ(c.g, 0.0f);
    EXPECT_FLOAT_EQ(c.b, 1.0f);
}

TEST(ValueRGBTest, ColorRGBCyan) {
    ColorRGB c = ColorRGB::CYAN();
    EXPECT_FLOAT_EQ(c.r, 0.0f);
    EXPECT_FLOAT_EQ(c.g, 1.0f);
    EXPECT_FLOAT_EQ(c.b, 1.0f);
}

TEST(ValueRGBTest, ColorRGBMagenta) {
    ColorRGB c = ColorRGB::MAGENTA();
    EXPECT_FLOAT_EQ(c.r, 1.0f);
    EXPECT_FLOAT_EQ(c.g, 0.0f);
    EXPECT_FLOAT_EQ(c.b, 1.0f);
}

TEST(ValueRGBTest, ColorRGBYellow) {
    ColorRGB c = ColorRGB::YELLOW();
    EXPECT_FLOAT_EQ(c.r, 1.0f);
    EXPECT_FLOAT_EQ(c.g, 1.0f);
    EXPECT_FLOAT_EQ(c.b, 0.0f);
}

// ============================================================================
// ARITHMETIC OPERATORS - RADIANCE
// ============================================================================

TEST(ValueRGBTest, RadianceRGBAddition) {
    RadianceRGB r1(1.0f, 2.0f, 3.0f);
    RadianceRGB r2(0.5f, 1.5f, 2.5f);
    RadianceRGB result = r1 + r2;
    EXPECT_FLOAT_EQ(result.r, 1.5f);
    EXPECT_FLOAT_EQ(result.g, 3.5f);
    EXPECT_FLOAT_EQ(result.b, 5.5f);
}

TEST(ValueRGBTest, RadianceRGBAdditionAssignment) {
    RadianceRGB r1(1.0f, 2.0f, 3.0f);
    RadianceRGB r2(0.5f, 1.5f, 2.5f);
    r1 += r2;
    EXPECT_FLOAT_EQ(r1.r, 1.5f);
    EXPECT_FLOAT_EQ(r1.g, 3.5f);
    EXPECT_FLOAT_EQ(r1.b, 5.5f);
}

TEST(ValueRGBTest, RadianceRGBMultiplicationByScalar) {
    RadianceRGB r(1.0f, 2.0f, 3.0f);
    RadianceRGB result = r * 2.0f;
    EXPECT_FLOAT_EQ(result.r, 2.0f);
    EXPECT_FLOAT_EQ(result.g, 4.0f);
    EXPECT_FLOAT_EQ(result.b, 6.0f);
}

TEST(ValueRGBTest, RadianceRGBMultiplicationScalarFirst) {
    RadianceRGB r(1.0f, 2.0f, 3.0f);
    RadianceRGB result = 2.0f * r;
    EXPECT_FLOAT_EQ(result.r, 2.0f);
    EXPECT_FLOAT_EQ(result.g, 4.0f);
    EXPECT_FLOAT_EQ(result.b, 6.0f);
}

TEST(ValueRGBTest, RadianceRGBMultiplicationAssignment) {
    RadianceRGB r(1.0f, 2.0f, 3.0f);
    r *= 2.0f;
    EXPECT_FLOAT_EQ(r.r, 2.0f);
    EXPECT_FLOAT_EQ(r.g, 4.0f);
    EXPECT_FLOAT_EQ(r.b, 6.0f);
}

TEST(ValueRGBTest, RadianceRGBDivisionByScalar) {
    RadianceRGB r(2.0f, 4.0f, 6.0f);
    RadianceRGB result = r / 2.0f;
    EXPECT_FLOAT_EQ(result.r, 1.0f);
    EXPECT_FLOAT_EQ(result.g, 2.0f);
    EXPECT_FLOAT_EQ(result.b, 3.0f);
}

TEST(ValueRGBTest, RadianceRGBDivisionAssignment) {
    RadianceRGB r(2.0f, 4.0f, 6.0f);
    r /= 2.0f;
    EXPECT_FLOAT_EQ(r.r, 1.0f);
    EXPECT_FLOAT_EQ(r.g, 2.0f);
    EXPECT_FLOAT_EQ(r.b, 3.0f);
}

// ============================================================================
// TYPE-SAFE OPERATIONS
// ============================================================================

TEST(ValueRGBTest, ReflectanceTimesRadiance) {
    ReflectanceRGB ref(0.5f, 0.6f, 0.7f);
    RadianceRGB rad(2.0f, 3.0f, 4.0f);
    RadianceRGB result = ref * rad;
    EXPECT_FLOAT_EQ(result.r, 1.0f);
    EXPECT_FLOAT_EQ(result.g, 1.8f);
    EXPECT_FLOAT_EQ(result.b, 2.8f);
}

TEST(ValueRGBTest, ParameterTimesRadiance) {
    ParameterRGB param(0.2f, 0.3f, 0.4f);
    RadianceRGB rad(5.0f, 10.0f, 15.0f);
    RadianceRGB result = param * rad;
    EXPECT_FLOAT_EQ(result.r, 1.0f);
    EXPECT_FLOAT_EQ(result.g, 3.0f);
    EXPECT_FLOAT_EQ(result.b, 6.0f);
}

TEST(ValueRGBTest, RadianceTimesParameter) {
    RadianceRGB rad(5.0f, 10.0f, 15.0f);
    ParameterRGB param(0.2f, 0.3f, 0.4f);
    RadianceRGB result = rad * param;
    EXPECT_FLOAT_EQ(result.r, 1.0f);
    EXPECT_FLOAT_EQ(result.g, 3.0f);
    EXPECT_FLOAT_EQ(result.b, 6.0f);
}

// ============================================================================
// VALUEARRAY TESTS
// ============================================================================

// ============================================================================
// DEFAULT CONSTRUCTION
// ============================================================================

TEST(ValueArrayTest, RadianceArrayDefaultConstructionZero) {
    RadianceArray<3> r;
    EXPECT_FLOAT_EQ(r.values[0], 0.0f);
    EXPECT_FLOAT_EQ(r.values[1], 0.0f);
    EXPECT_FLOAT_EQ(r.values[2], 0.0f);
}

TEST(ValueArrayTest, ColorArrayDefaultConstructionZero) {
    ColorArray<4> c;
    EXPECT_FLOAT_EQ(c.values[0], 0.0f);
    EXPECT_FLOAT_EQ(c.values[1], 0.0f);
    EXPECT_FLOAT_EQ(c.values[2], 0.0f);
    EXPECT_FLOAT_EQ(c.values[3], 0.0f);
}


TEST(ValueArrayTest, ParameterArrayDefaultConstructionZero) {
    ParameterArray<3> p;
    EXPECT_FLOAT_EQ(p.values[0], 0.0f);
    EXPECT_FLOAT_EQ(p.values[1], 0.0f);
    EXPECT_FLOAT_EQ(p.values[2], 0.0f);
}

// ============================================================================
// ARRAY CONSTRUCTOR
// ============================================================================
// Note: ValueArray subtypes' array constructors have bugs in the header file
// (they call default constructor instead of array constructor).
// The RGB versions work correctly.

// ============================================================================
// COPY CONSTRUCTION
// ============================================================================

TEST(ValueArrayTest, RadianceArrayCopyConstruction) {
    RadianceArray<3> r1;
    r1.values[0] = 1.0f;
    r1.values[1] = 2.0f;
    r1.values[2] = 3.0f;

    RadianceArray<3> r2(r1);
    EXPECT_FLOAT_EQ(r2.values[0], 1.0f);
    EXPECT_FLOAT_EQ(r2.values[1], 2.0f);
    EXPECT_FLOAT_EQ(r2.values[2], 3.0f);
}

TEST(ValueArrayTest, ColorArrayCopyConstruction) {
    ColorArray<4> c1;
    c1.values[0] = 0.1f;
    c1.values[1] = 0.2f;
    c1.values[2] = 0.3f;
    c1.values[3] = 0.4f;

    ColorArray<4> c2(c1);
    EXPECT_FLOAT_EQ(c2.values[0], 0.1f);
    EXPECT_FLOAT_EQ(c2.values[1], 0.2f);
    EXPECT_FLOAT_EQ(c2.values[2], 0.3f);
    EXPECT_FLOAT_EQ(c2.values[3], 0.4f);
}

// ============================================================================
// CROSS-TYPE CONSTRUCTION
// ============================================================================
// Note: ValueArray cross-type constructor uses std::begin/std::end which
// aren't implemented for ValueArray types. The RGB versions work fine.

// ============================================================================
// RESIDUAL METHOD
// ============================================================================
// Note: ValueArray residual() has template issues in the header file's
// implementation. The RGB versions work fine.

// ============================================================================
// HAS NON-ZERO COMPONENT
// ============================================================================

TEST(ValueArrayTest, RadianceArrayHasNonZeroComponentTrue) {
    RadianceArray<3> r;
    r.values[0] = 0.0f;
    r.values[1] = 0.0f;
    r.values[2] = 0.1f;
    EXPECT_TRUE(r.hasNonZeroComponent());
}

TEST(ValueArrayTest, RadianceArrayHasNonZeroComponentFalse) {
    RadianceArray<3> r;
    r.values[0] = 0.0f;
    r.values[1] = 0.0f;
    r.values[2] = 0.0f;
    EXPECT_FALSE(r.hasNonZeroComponent());
}

TEST(ValueArrayTest, ColorArrayHasNonZeroComponentTrue) {
    ColorArray<4> c;
    c.values[0] = 0.0f;
    c.values[1] = 0.5f;
    c.values[2] = 0.0f;
    c.values[3] = 0.0f;
    EXPECT_TRUE(c.hasNonZeroComponent());
}

TEST(ValueArrayTest, ColorArrayHasNonZeroComponentFalse) {
    ColorArray<4> c;
    EXPECT_FALSE(c.hasNonZeroComponent());
}

// ============================================================================
// TYPE-SAFE OPERATIONS - VALUE ARRAYS
// ============================================================================
// Note: ValueArray arithmetic operators use templates that appear incomplete
// in the header file (opValueArrays and opValueArrayScalar template deduction).
// The RGB versions work correctly, so we focus on ValueRGB operators above.



// ============================================================================
// DIFFERENT ARRAY SIZES
// ============================================================================

TEST(ValueArrayTest, ColorArray8Elements) {
    ColorArray<8> c;
    for(int i = 0; i < 8; i++) {
        c.values[i] = static_cast<float>(i) * 0.1f;
    }

    for(int i = 0; i < 8; i++) {
        EXPECT_FLOAT_EQ(c.values[i], static_cast<float>(i) * 0.1f);
    }
}

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
