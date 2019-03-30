#include <gtest/gtest.h>
#include "rng.h"

namespace {

TEST(RandomTest, Uniform01) {
    RNG rng;
    for(int i = 0; i < 100; ++i) {
        float e = rng.uniform01();
        EXPECT_GE(e, 0.0f);
        EXPECT_LE(e, 1.0f);
    }
}

TEST(RandomTest, UniformRange) {
    RNG rng;
    for(int i = -100; i < 100; ++i) {
        float low = (float) i;
        float high = (float) i + 10.0f;
        float e = rng.uniformRange(low, high);
        EXPECT_GE(e, low);
        EXPECT_LE(e, high);
    }
}

TEST(RandomTest, UniformUnitCircle) {
    RNG rng;
    float x, y;
    for(int i = 0; i < 100; ++i) {
        rng.uniformUnitCircle(x, y);
        EXPECT_GE(x, -1.0f);
        EXPECT_LE(x, +1.0f);
        EXPECT_GE(y, -1.0f);
        EXPECT_LE(y, +1.0f);
        float len = std::sqrt(x * x + y * y);
        EXPECT_LE(len, 1.0f);
    }
}

TEST(RandomTest, UniformCircle) {
    RNG rng;
    float x, y, r;
    for(int i = 0; i < 100; ++i) {
        r = rng.uniformRange(0.0f, 10.0f);
        rng.uniformCircle(r, x, y);
        EXPECT_GE(x, -r);
        EXPECT_LE(x, +r);
        EXPECT_GE(y, -r);
        EXPECT_LE(y, +r);
        float len = std::sqrt(x * x + y * y);
        EXPECT_LE(len, +r);
    }
}

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

