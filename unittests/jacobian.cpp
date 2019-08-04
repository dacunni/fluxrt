#include <gtest/gtest.h>
#include <cmath>
#include "jacobian.h"

namespace {

TEST(JacobianAreaWarpTest, JacobianAnalytic2DUniform) {
    using namespace jacobian::differential;
    // Parameters: dx/du dx/dv dy/du dy/dv

    // No distortion
    // x = u, y = v
    EXPECT_FLOAT_EQ(1, from2Dto2D(1, 0, 0, 1));
    // x = v, y = u
    EXPECT_FLOAT_EQ(1, from2Dto2D(0, 1, 1, 0));
    // x = -u, y = v
    EXPECT_FLOAT_EQ(1, from2Dto2D(-1, 0, 0, 1));
    // x = -u, y = -v
    EXPECT_FLOAT_EQ(1, from2Dto2D(-1, 0, 0, -1));
    // x = u, y = -v
    EXPECT_FLOAT_EQ(1, from2Dto2D(1, 0, 0, -1));
}

TEST(JacobianAreaWarpTest, JacobianAnalytic2DNonUniform) {
    using namespace jacobian::differential;
    // Parameters: dx/du dx/dv dy/du dy/dv

    // Non-uniform scaling
    // x = 2u, y = v
    EXPECT_FLOAT_EQ(2, from2Dto2D(2, 0, 0, 1));
    // x = u, y = 2v
    EXPECT_FLOAT_EQ(2, from2Dto2D(1, 0, 0, 2));
    // x = 3u, y = 2v
    EXPECT_FLOAT_EQ(6, from2Dto2D(3, 0, 0, 2));
    // x = -3u, y = 2v
    EXPECT_FLOAT_EQ(6, from2Dto2D(-3, 0, 0, 2));
    // x = 3u, y = -2v
    EXPECT_FLOAT_EQ(6, from2Dto2D(3, 0, 0, -2));
    // x = -3u, y = -2v
    EXPECT_FLOAT_EQ(6, from2Dto2D(-3, 0, 0, -2));
    // x = 3v, y = 2u
    EXPECT_FLOAT_EQ(6, from2Dto2D(0, 3, 2, 0));

    // Non-uniform scaling (generalized)
    // x = 4u + 3v   dx/du = 4  dx/dv = 3
    // y = 2u + 5v   dy/du = 2  dy/dv = 5
    EXPECT_FLOAT_EQ(14, from2Dto2D(4, 3, 2, 5));
}

TEST(JacobianAreaWarpTest, JacobianAnalytic3DUniform) {
    using namespace jacobian::differential;
    // Parameters: dx/du dx/dv dx/dw dy/du dy/dv dy/dw dz/du dy/dv dz/dw

    // No distortion
    // x = u, y = v, z = w
    EXPECT_FLOAT_EQ(1, from3Dto3D(1, 0, 0, 0, 1, 0, 0, 0, 1));
    // x = w, y = v, z = u
    EXPECT_FLOAT_EQ(1, from3Dto3D(0, 0, 1, 0, 1, 0, 1, 0, 0));
    // x = v, y = w, z = u
    EXPECT_FLOAT_EQ(1, from3Dto3D(0, 1, 0, 0, 0, 1, 1, 0, 0));
    // x = -v, y = w, z = -u
    EXPECT_FLOAT_EQ(1, from3Dto3D(0, -1, 0, 0, 0, 1, -1, 0, 0));
}

TEST(JacobianAreaWarpTest, JacobianAnalytic3DNonUniform) {
    using namespace jacobian::differential;
    // Parameters: dx/du dx/dv dx/dw dy/du dy/dv dy/dw dz/du dy/dv dz/dw

    // Non-uniform scaling
    // x = 5u, y = 7v, z = 3w
    EXPECT_FLOAT_EQ(105, from3Dto3D(5, 0, 0, 0, 7, 0, 0, 0, 3));
    // x = 5u, y = -7v, z = 3w
    EXPECT_FLOAT_EQ(105, from3Dto3D(5, 0, 0, 0, -7, 0, 0, 0, 3));
    // x = -5u, y = 7v, z = 3w
    EXPECT_FLOAT_EQ(105, from3Dto3D(-5, 0, 0, 0, 7, 0, 0, 0, 3));
    // x = 5u, y = 7v, z = -3w
    EXPECT_FLOAT_EQ(105, from3Dto3D(5, 0, 0, 0, 7, 0, 0, 0, -3));
    // x = -5u, y = -7v, z = -3w
    EXPECT_FLOAT_EQ(105, from3Dto3D(-5, 0, 0, 0, -7, 0, 0, 0, -3));

    // Non-uniform scaling (generalized)
    // x = 4u + 3v + 2w  dx/du = 4  dx/dv = 3  dx/dw = 2
    // y = 2u + 5v + 3w  dy/du = 2  dy/dv = 5  dx/dw = 3
    // z =  u + 2v + 8w  dy/du = 1  dy/dv = 2  dx/dw = 8
    EXPECT_FLOAT_EQ(95, from3Dto3D(4, 3, 2, 2, 5, 3, 1, 2, 8));
}

TEST(JacobianAreaWarpTest, JacobianNumerical2DUniform) {
    using namespace jacobian::numerical;
    // Parameters: x(), y(), u, v, du, dv

    const float closeness = 1.0e-3;
    float du = 0.01f;
    float dv = 0.01f;

    {
    auto x = [](float u, float v) { return u; };
    auto y = [](float u, float v) { return v; };
    EXPECT_NEAR(1, from2Dto2D(x, y, 0, 0, du, dv), closeness);
    EXPECT_NEAR(1, from2Dto2D(x, y, 1, 1, du, dv), closeness);
    EXPECT_NEAR(1, from2Dto2D(x, y, -1, 1, du, dv), closeness);
    EXPECT_NEAR(1, from2Dto2D(x, y, 1, -1, du, dv), closeness);
    EXPECT_NEAR(1, from2Dto2D(x, y, -1, -1, du, dv), closeness);
    EXPECT_NEAR(1, from2Dto2D(x, y, 3, 2, du, dv), closeness);
    }

    {
    auto x = [](float u, float v) { return v; };
    auto y = [](float u, float v) { return u; };
    EXPECT_NEAR(1, from2Dto2D(x, y, 0, 0, du, dv), closeness);
    EXPECT_NEAR(1, from2Dto2D(x, y, 1, 1, du, dv), closeness);
    EXPECT_NEAR(1, from2Dto2D(x, y, -1, 1, du, dv), closeness);
    EXPECT_NEAR(1, from2Dto2D(x, y, 1, -1, du, dv), closeness);
    EXPECT_NEAR(1, from2Dto2D(x, y, -1, -1, du, dv), closeness);
    EXPECT_NEAR(1, from2Dto2D(x, y, 3, 2, du, dv), closeness);
    }

    {
    auto x = [](float u, float v) { return -u; };
    auto y = [](float u, float v) { return -v; };
    EXPECT_NEAR(1, from2Dto2D(x, y, 0, 0, du, dv), closeness);
    EXPECT_NEAR(1, from2Dto2D(x, y, 1, 1, du, dv), closeness);
    EXPECT_NEAR(1, from2Dto2D(x, y, -1, 1, du, dv), closeness);
    EXPECT_NEAR(1, from2Dto2D(x, y, 1, -1, du, dv), closeness);
    EXPECT_NEAR(1, from2Dto2D(x, y, -1, -1, du, dv), closeness);
    EXPECT_NEAR(1, from2Dto2D(x, y, 3, 2, du, dv), closeness);
    }

    {
    auto x = [](float u, float v) { return u; };
    auto y = [](float u, float v) { return -v; };
    EXPECT_NEAR(1, from2Dto2D(x, y, 0, 0, du, dv), closeness);
    EXPECT_NEAR(1, from2Dto2D(x, y, 1, 1, du, dv), closeness);
    EXPECT_NEAR(1, from2Dto2D(x, y, -1, 1, du, dv), closeness);
    EXPECT_NEAR(1, from2Dto2D(x, y, 1, -1, du, dv), closeness);
    EXPECT_NEAR(1, from2Dto2D(x, y, -1, -1, du, dv), closeness);
    EXPECT_NEAR(1, from2Dto2D(x, y, 3, 2, du, dv), closeness);
    }
}

TEST(JacobianAreaWarpTest, JacobianNumerical2DNonUniform) {
    using namespace jacobian::numerical;
    // Parameters: x(), y(), u, v, du, dv

    const float closeness = 1.0e-1;
    float du = 1.0e-3f;
    float dv = 1.0e-3f;

    {
    // dx/du = 2u  dx/dv = 0
    // dy/du = 0   dy/dv = 2v
    auto x = [](float u, float v) { return u * u; };
    auto y = [](float u, float v) { return v * v; };
    EXPECT_NEAR( 0, from2Dto2D(x, y , 0,  0, du, dv), closeness);
    EXPECT_NEAR( 4, from2Dto2D(x, y , 1,  1, du, dv), closeness);
    EXPECT_NEAR( 4, from2Dto2D(x, y, -1,  1, du, dv), closeness);
    EXPECT_NEAR( 4, from2Dto2D(x, y , 1, -1, du, dv), closeness);
    EXPECT_NEAR( 4, from2Dto2D(x, y, -1, -1, du, dv), closeness);
    EXPECT_NEAR(24, from2Dto2D(x, y , 3,  2, du, dv), closeness);
    }

    {
    // dx/du = 2u  dx/dv = 3
    // dy/du = 5   dy/dv = 2v
    auto x = [](float u, float v) { return u * u + 3 * v; };
    auto y = [](float u, float v) { return v * v + 5 * u; };
    EXPECT_NEAR(15, from2Dto2D(x, y,  0,  0, du, dv), closeness);
    EXPECT_NEAR(11, from2Dto2D(x, y,  1,  1, du, dv), closeness);
    EXPECT_NEAR(19, from2Dto2D(x, y, -1,  1, du, dv), closeness);
    EXPECT_NEAR(19, from2Dto2D(x, y,  1, -1, du, dv), closeness);
    EXPECT_NEAR(11, from2Dto2D(x, y, -1, -1, du, dv), closeness);
    EXPECT_NEAR( 9, from2Dto2D(x, y,  3,  2, du, dv), closeness);
    }

}

// TODO - 3D Uniform
// TODO - 3D Non-Uniform

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

