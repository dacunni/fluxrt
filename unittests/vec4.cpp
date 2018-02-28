#include <gtest/gtest.h>
#include "micromath.h"

namespace {

#if 0
TEST(Vec4Test, Addition) {
    vec4 a, b, r;
    a.set(3, -4, 5); b.set(2, 7, -1); r.set(a.x + b.x, a.y + b.y, a.z + b.z);
    EXPECT_EQ(a + b, r);
    a.set(0, 0, 0); b.set(0, 0, 0); r.set(a.x + b.x, a.y + b.y, a.z + b.z);
    EXPECT_EQ(a + b, r);
    a.set(1000, 1000, 1000); b.set(-1000, -1000, -1000); r.set(a.x + b.x, a.y + b.y, a.z + b.z);
    EXPECT_EQ(a + b, r);
}

TEST(Vec4Test, Subtraction) {
    vec4 a, b, r;
    a.set(3, -4, 5); b.set(2, 7, -1); r.set(a.x - b.x, a.y - b.y, a.z - b.z);
    EXPECT_EQ(a - b, r);
    a.set(0, 0, 0); b.set(0, 0, 0); r.set(a.x - b.x, a.y - b.y, a.z - b.z);
    EXPECT_EQ(a - b, r);
    a.set(1000, 1000, 1000); b.set(-1000, -1000, -1000); r.set(a.x - b.x, a.y - b.y, a.z - b.z);
    EXPECT_EQ(a - b, r);
}

TEST(Vec4Test, DotProduct) {
    // Parallel vectors
    EXPECT_FLOAT_EQ(dot(vec4(1, 0, 0), vec4(1, 0, 0)), 1);
    EXPECT_FLOAT_EQ(dot(vec4(0, 1, 0), vec4(0, 1, 0)), 1);
    EXPECT_FLOAT_EQ(dot(vec4(0, 0, 1), vec4(0, 0, 1)), 1);
    EXPECT_FLOAT_EQ(dot(vec4(9, 0, 0), vec4(1, 0, 0)), 9);
    EXPECT_FLOAT_EQ(dot(vec4(0, 9, 0), vec4(0, 1, 0)), 9);
    EXPECT_FLOAT_EQ(dot(vec4(0, 0, 9), vec4(0, 0, 1)), 9);
    EXPECT_FLOAT_EQ(dot(vec4(1, 0, 0), vec4(9, 0, 0)), 9);
    EXPECT_FLOAT_EQ(dot(vec4(0, 1, 0), vec4(0, 9, 0)), 9);
    EXPECT_FLOAT_EQ(dot(vec4(0, 0, 1), vec4(0, 0, 9)), 9);

    // Perpendicular Vectors
    EXPECT_FLOAT_EQ(dot(vec4(  1,  0,  0), vec4( 0,  1,  0)), 0);
    EXPECT_FLOAT_EQ(dot(vec4(  1,  0,  0), vec4( 0, -1,  0)), 0);
    EXPECT_FLOAT_EQ(dot(vec4( -1,  0,  0), vec4( 0,  1,  0)), 0);
    EXPECT_FLOAT_EQ(dot(vec4( -1,  0,  0), vec4( 0, -1,  0)), 0);
    EXPECT_FLOAT_EQ(dot(vec4(  1,  0,  0), vec4( 0,  0,  1)), 0);
    EXPECT_FLOAT_EQ(dot(vec4(  1,  0,  0), vec4( 0,  0, -1)), 0);
    EXPECT_FLOAT_EQ(dot(vec4( -1,  0,  0), vec4( 0,  0,  1)), 0);
    EXPECT_FLOAT_EQ(dot(vec4( -1,  0,  0), vec4( 0,  0, -1)), 0);
    EXPECT_FLOAT_EQ(dot(vec4(  0,  1,  0), vec4( 0,  0,  1)), 0);
    EXPECT_FLOAT_EQ(dot(vec4(  0,  1,  0), vec4( 0,  0, -1)), 0);
    EXPECT_FLOAT_EQ(dot(vec4(  0, -1,  0), vec4( 0,  0,  1)), 0);
    EXPECT_FLOAT_EQ(dot(vec4(  0, -1,  0), vec4( 0,  0, -1)), 0);
    EXPECT_FLOAT_EQ(dot(vec4(  7,  0,  0), vec4( 0,  3,  0)), 0);
    EXPECT_FLOAT_EQ(dot(vec4(  7,  0,  0), vec4( 0, -3,  0)), 0);
    EXPECT_FLOAT_EQ(dot(vec4( -7,  0,  0), vec4( 0,  3,  0)), 0);
    EXPECT_FLOAT_EQ(dot(vec4( -7,  0,  0), vec4( 0, -3,  0)), 0);
    EXPECT_FLOAT_EQ(dot(vec4(  7,  0,  0), vec4( 0,  0,  3)), 0);
    EXPECT_FLOAT_EQ(dot(vec4(  7,  0,  0), vec4( 0,  0, -3)), 0);
    EXPECT_FLOAT_EQ(dot(vec4( -7,  0,  0), vec4( 0,  0,  3)), 0);
    EXPECT_FLOAT_EQ(dot(vec4( -7,  0,  0), vec4( 0,  0, -3)), 0);
    EXPECT_FLOAT_EQ(dot(vec4(  0,  7,  0), vec4( 0,  0,  3)), 0);
    EXPECT_FLOAT_EQ(dot(vec4(  0,  7,  0), vec4( 0,  0, -3)), 0);
    EXPECT_FLOAT_EQ(dot(vec4(  0, -7,  0), vec4( 0,  0,  3)), 0);
    EXPECT_FLOAT_EQ(dot(vec4(  0, -7,  0), vec4( 0,  0, -3)), 0);

    // Unit Vectors pi/4 Radians Apart
    {
    const float s = 1.0f / sqrtf(2.0f);
    const float r = cosf(M_PI / 4.0f);
    EXPECT_FLOAT_EQ(dot(vec4(s, 0, s), vec4(1, 0, 0)), r);
    EXPECT_FLOAT_EQ(dot(vec4(s, s, 0), vec4(1, 0, 0)), r);
    EXPECT_FLOAT_EQ(dot(vec4(s, s, 0), vec4(0, 1, 0)), r);
    EXPECT_FLOAT_EQ(dot(vec4(0, s, s), vec4(0, 1, 0)), r);
    EXPECT_FLOAT_EQ(dot(vec4(0, s, s), vec4(0, 0, 1)), r);
    EXPECT_FLOAT_EQ(dot(vec4(s, 0, s), vec4(0, 0, 1)), r);
    }

    // Different Length Vectors pi/4 Radians Apart
    {
    const float a = 4.52, b = 9.42;
    const float s = a / sqrtf(2.0f);
    const float r = a * b * cosf(M_PI / 4.0f);
    EXPECT_FLOAT_EQ(dot(vec4(s, 0, s), vec4(b, 0, 0)), r);
    EXPECT_FLOAT_EQ(dot(vec4(s, s, 0), vec4(b, 0, 0)), r);
    EXPECT_FLOAT_EQ(dot(vec4(s, s, 0), vec4(0, b, 0)), r);
    EXPECT_FLOAT_EQ(dot(vec4(0, s, s), vec4(0, b, 0)), r);
    EXPECT_FLOAT_EQ(dot(vec4(0, s, s), vec4(0, 0, b)), r);
    EXPECT_FLOAT_EQ(dot(vec4(s, 0, s), vec4(0, 0, b)), r);
    }
}

TEST(Vec4Test, Mirror) {
    vec4 a;
    a.set(3, -4, 5, 0);
    EXPECT_FLOAT_EQ(dot(mirror(a, vec4( 1,  0,  0, 0)), vec4( a.x, -a.y, -a.z, 0)), a.magnitude_sq());
    EXPECT_FLOAT_EQ(dot(mirror(a, vec4(-1,  0,  0, 0)), vec4( a.x, -a.y, -a.z, 0)), a.magnitude_sq());
    EXPECT_FLOAT_EQ(dot(mirror(a, vec4( 0,  1,  0, 0)), vec4(-a.x,  a.y, -a.z, 0)), a.magnitude_sq());
    EXPECT_FLOAT_EQ(dot(mirror(a, vec4( 0, -1,  0, 0)), vec4(-a.x,  a.y, -a.z, 0)), a.magnitude_sq());
    EXPECT_FLOAT_EQ(dot(mirror(a, vec4( 0,  0,  1, 0)), vec4(-a.x, -a.y,  a.z, 0)), a.magnitude_sq());
    EXPECT_FLOAT_EQ(dot(mirror(a, vec4( 0,  0, -1, 0)), vec4(-a.x, -a.y,  a.z, 0)), a.magnitude_sq());

    const float s = 1.0f / sqrtf(2.0f);
    a.set(s, s, 0);
    EXPECT_FLOAT_EQ(dot(mirror(vec4( 1,  0,  0, 0), a), vec4( 0,  1,  0, 0)), a.magnitude_sq());
    EXPECT_FLOAT_EQ(dot(mirror(vec4( 0,  1,  0, 0), a), vec4( 1,  0,  0, 0)), a.magnitude_sq());
    EXPECT_FLOAT_EQ(dot(mirror(vec4( 0,  0,  1, 0), a), vec4( 0,  0, -1, 0)), a.magnitude_sq());
}
#endif

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

