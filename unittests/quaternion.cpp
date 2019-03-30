#include <gtest/gtest.h>
#include "quaternion.h"

namespace {

TEST(QuaternionTest, Init) {
    {
        quaternion q;
        EXPECT_FLOAT_EQ(q.x, 0.0f);
        EXPECT_FLOAT_EQ(q.y, 0.0f);
        EXPECT_FLOAT_EQ(q.z, 0.0f);
        EXPECT_FLOAT_EQ(q.r, 1.0f);
    }
    {
        quaternion q(2.0f, 3.0f, 4.0f);
        EXPECT_FLOAT_EQ(q.x, 2.0f);
        EXPECT_FLOAT_EQ(q.y, 3.0f);
        EXPECT_FLOAT_EQ(q.z, 4.0f);
        EXPECT_FLOAT_EQ(q.r, 1.0f);
    }
    {
        quaternion q(2.0f, 3.0f, 4.0f, 5.0f);
        EXPECT_FLOAT_EQ(q.x, 2.0f);
        EXPECT_FLOAT_EQ(q.y, 3.0f);
        EXPECT_FLOAT_EQ(q.z, 4.0f);
        EXPECT_FLOAT_EQ(q.r, 5.0f);
    }
    {
        quaternion p(2.0f, 3.0f, 4.0f, 5.0f);
        quaternion q = p;
        EXPECT_FLOAT_EQ(q.x, 2.0f);
        EXPECT_FLOAT_EQ(q.y, 3.0f);
        EXPECT_FLOAT_EQ(q.z, 4.0f);
        EXPECT_FLOAT_EQ(q.r, 5.0f);
    }

    // TODO - axis angle, inc with unnormalized axis
}

TEST(QuaternionTest, Set) {
    quaternion q;
    q.set(2.0f, 3.0f, 4.0f, 5.0f);
    EXPECT_FLOAT_EQ(q.x, 2.0f);
    EXPECT_FLOAT_EQ(q.y, 3.0f);
    EXPECT_FLOAT_EQ(q.z, 4.0f);
    EXPECT_FLOAT_EQ(q.r, 5.0f);
}

TEST(QuaternionTest, Norm) {
    quaternion q(2.0f, 3.0f, 4.0f, 5.0f);
    EXPECT_FLOAT_EQ(norm(q),
                    std::sqrt(q.x * q.x + q.y * q.y +
                              q.z * q.z + q.r * q.r));
}

TEST(QuaternionTest, Conjugate) {
    quaternion q(2.0f, 3.0f, 4.0f, 5.0f);
    quaternion c = conjugate(q);
    EXPECT_FLOAT_EQ(c.x, -q.x);
    EXPECT_FLOAT_EQ(c.y, -q.y);
    EXPECT_FLOAT_EQ(c.z, -q.z);
    EXPECT_FLOAT_EQ(c.r,  q.r);
}

TEST(QuaternionTest, Multiplication) {
    quaternion a(1.4, 1.3, 2.3, 4.6);
    quaternion b(-0.3, 1.8, -0.5, 1.7);
    quaternion c = mult(a, b);
    float r = a.r * b.r - a.x * b.x - a.y * b.y - a.z * b.z;
    float x = a.r * b.x + a.x * b.r + a.y * b.z - a.z * b.y;
    float y = a.r * b.y - a.x * b.z + a.y * b.r + a.z * b.x;
    float z = a.r * b.z + a.x * b.y - a.y * b.x + a.z * b.r;
    EXPECT_FLOAT_EQ(c.x, x);
    EXPECT_FLOAT_EQ(c.y, y);
    EXPECT_FLOAT_EQ(c.z, z);
    EXPECT_FLOAT_EQ(c.r, r);
}

#define EXPECT_VEC3_EQ(a, b) \
    EXPECT_FLOAT_EQ(a.x, b.x); \
    EXPECT_FLOAT_EQ(a.y, b.y); \
    EXPECT_FLOAT_EQ(a.z, b.z)

// Rotations of pi/2 of axis vectors, using a quaternion
// constructed with a normalized rotation axis.
TEST(QuaternionTest, RotationRightAnglesUnitAxis) {
    vec3 upx( 1, 0, 0), upy(0,  1, 0), upz(0, 0,  1);
    vec3 unx(-1, 0, 0), uny(0, -1, 0), unz(0, 0, -1);
    quaternion q;
    vec3 r;

    // Rotation about +x axis by pi/2
    q = quaternion::unitAxisAngle(upx, 0.5 * M_PI);
    r = rotate(q, upx); EXPECT_VEC3_EQ(upx, r);
    r = rotate(q, upy); EXPECT_VEC3_EQ(upz, r);
    r = rotate(q, upz); EXPECT_VEC3_EQ(uny, r);
    r = rotate(q, unx); EXPECT_VEC3_EQ(unx, r);
    r = rotate(q, uny); EXPECT_VEC3_EQ(unz, r);
    r = rotate(q, unz); EXPECT_VEC3_EQ(upy, r);

    // Rotation about +y axis by pi/2
    q = quaternion::unitAxisAngle(upy, 0.5 * M_PI);
    r = rotate(q, upx); EXPECT_VEC3_EQ(unz, r);
    r = rotate(q, upy); EXPECT_VEC3_EQ(upy, r);
    r = rotate(q, upz); EXPECT_VEC3_EQ(upx, r);
    r = rotate(q, unx); EXPECT_VEC3_EQ(upz, r);
    r = rotate(q, uny); EXPECT_VEC3_EQ(uny, r);
    r = rotate(q, unz); EXPECT_VEC3_EQ(unx, r);

    // Rotation about +z axis by pi/2
    q = quaternion::unitAxisAngle(upz, 0.5 * M_PI);
    r = rotate(q, upx); EXPECT_VEC3_EQ(upy, r);
    r = rotate(q, upy); EXPECT_VEC3_EQ(unx, r);
    r = rotate(q, upz); EXPECT_VEC3_EQ(upz, r);
    r = rotate(q, unx); EXPECT_VEC3_EQ(uny, r);
    r = rotate(q, uny); EXPECT_VEC3_EQ(upx, r);
    r = rotate(q, unz); EXPECT_VEC3_EQ(unz, r);

    // Rotation about -x axis by pi/2
    q = quaternion::unitAxisAngle(unx, 0.5 * M_PI);
    r = rotate(q, upx); EXPECT_VEC3_EQ(upx, r);
    r = rotate(q, upy); EXPECT_VEC3_EQ(unz, r);
    r = rotate(q, upz); EXPECT_VEC3_EQ(upy, r);
    r = rotate(q, unx); EXPECT_VEC3_EQ(unx, r);
    r = rotate(q, uny); EXPECT_VEC3_EQ(upz, r);
    r = rotate(q, unz); EXPECT_VEC3_EQ(uny, r);

    // Rotation about -y axis by pi/2
    q = quaternion::unitAxisAngle(uny, 0.5 * M_PI);
    r = rotate(q, upx); EXPECT_VEC3_EQ(upz, r);
    r = rotate(q, upy); EXPECT_VEC3_EQ(upy, r);
    r = rotate(q, upz); EXPECT_VEC3_EQ(unx, r);
    r = rotate(q, unx); EXPECT_VEC3_EQ(unz, r);
    r = rotate(q, uny); EXPECT_VEC3_EQ(uny, r);
    r = rotate(q, unz); EXPECT_VEC3_EQ(upx, r);

    // Rotation about -z axis by pi/2
    q = quaternion::unitAxisAngle(unz, 0.5 * M_PI);
    r = rotate(q, upx); EXPECT_VEC3_EQ(uny, r);
    r = rotate(q, upy); EXPECT_VEC3_EQ(upx, r);
    r = rotate(q, upz); EXPECT_VEC3_EQ(upz, r);
    r = rotate(q, unx); EXPECT_VEC3_EQ(upy, r);
    r = rotate(q, uny); EXPECT_VEC3_EQ(unx, r);
    r = rotate(q, unz); EXPECT_VEC3_EQ(unz, r);
}

// Rotations of pi/2 of axis vectors, using a quaternion
// constructed with an unnormalized rotation axis.
TEST(QuaternionTest, RotationRightAnglesUnnormalizedAxis) {
    vec3 upx( 1, 0, 0), upy(0,  1, 0), upz(0, 0,  1);
    vec3 unx(-1, 0, 0), uny(0, -1, 0), unz(0, 0, -1);
    vec3 spx( 3, 0, 0), spy(0,  3, 0), spz(0, 0,  3);
    vec3 snx(-3, 0, 0), sny(0, -3, 0), snz(0, 0, -3);
    quaternion q;
    vec3 r;

    // Rotation about +x axis by pi/2
    q = quaternion::axisAngle(spx, 0.5 * M_PI);
    r = rotate(q, upx); EXPECT_VEC3_EQ(upx, r);
    r = rotate(q, upy); EXPECT_VEC3_EQ(upz, r);
    r = rotate(q, upz); EXPECT_VEC3_EQ(uny, r);
    r = rotate(q, unx); EXPECT_VEC3_EQ(unx, r);
    r = rotate(q, uny); EXPECT_VEC3_EQ(unz, r);
    r = rotate(q, unz); EXPECT_VEC3_EQ(upy, r);

    // Rotation about +y axis by pi/2
    q = quaternion::axisAngle(spy, 0.5 * M_PI);
    r = rotate(q, upx); EXPECT_VEC3_EQ(unz, r);
    r = rotate(q, upy); EXPECT_VEC3_EQ(upy, r);
    r = rotate(q, upz); EXPECT_VEC3_EQ(upx, r);
    r = rotate(q, unx); EXPECT_VEC3_EQ(upz, r);
    r = rotate(q, uny); EXPECT_VEC3_EQ(uny, r);
    r = rotate(q, unz); EXPECT_VEC3_EQ(unx, r);

    // Rotation about +z axis by pi/2
    q = quaternion::axisAngle(spz, 0.5 * M_PI);
    r = rotate(q, upx); EXPECT_VEC3_EQ(upy, r);
    r = rotate(q, upy); EXPECT_VEC3_EQ(unx, r);
    r = rotate(q, upz); EXPECT_VEC3_EQ(upz, r);
    r = rotate(q, unx); EXPECT_VEC3_EQ(uny, r);
    r = rotate(q, uny); EXPECT_VEC3_EQ(upx, r);
    r = rotate(q, unz); EXPECT_VEC3_EQ(unz, r);

    // Rotation about -x axis by pi/2
    q = quaternion::axisAngle(snx, 0.5 * M_PI);
    r = rotate(q, upx); EXPECT_VEC3_EQ(upx, r);
    r = rotate(q, upy); EXPECT_VEC3_EQ(unz, r);
    r = rotate(q, upz); EXPECT_VEC3_EQ(upy, r);
    r = rotate(q, unx); EXPECT_VEC3_EQ(unx, r);
    r = rotate(q, uny); EXPECT_VEC3_EQ(upz, r);
    r = rotate(q, unz); EXPECT_VEC3_EQ(uny, r);

    // Rotation about -y axis by pi/2
    q = quaternion::axisAngle(sny, 0.5 * M_PI);
    r = rotate(q, upx); EXPECT_VEC3_EQ(upz, r);
    r = rotate(q, upy); EXPECT_VEC3_EQ(upy, r);
    r = rotate(q, upz); EXPECT_VEC3_EQ(unx, r);
    r = rotate(q, unx); EXPECT_VEC3_EQ(unz, r);
    r = rotate(q, uny); EXPECT_VEC3_EQ(uny, r);
    r = rotate(q, unz); EXPECT_VEC3_EQ(upx, r);

    // Rotation about -z axis by pi/2
    q = quaternion::axisAngle(snz, 0.5 * M_PI);
    r = rotate(q, upx); EXPECT_VEC3_EQ(uny, r);
    r = rotate(q, upy); EXPECT_VEC3_EQ(upx, r);
    r = rotate(q, upz); EXPECT_VEC3_EQ(upz, r);
    r = rotate(q, unx); EXPECT_VEC3_EQ(upy, r);
    r = rotate(q, uny); EXPECT_VEC3_EQ(unx, r);
    r = rotate(q, unz); EXPECT_VEC3_EQ(unz, r);
}

// TODO - more rotations, with and without unit rotation axes

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

