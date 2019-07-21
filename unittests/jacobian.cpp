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

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

