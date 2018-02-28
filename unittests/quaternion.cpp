#include <gtest/gtest.h>
#include "micromath.h"

namespace {

TEST(QuaternionTest, Multiplication) {
    quaternion a, b, r;
    // TODO
    r = mult(a, b);
    EXPECT_EQ(1, 0);
}

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

