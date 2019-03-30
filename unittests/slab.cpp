#include <gtest/gtest.h>
#include "slab.h"

namespace {

TEST(SlabInitTest, DefaultConstruct) {
    Slab a;
    EXPECT_FLOAT_EQ(a.xmin, 0.0f);
    EXPECT_FLOAT_EQ(a.ymin, 0.0f);
    EXPECT_FLOAT_EQ(a.zmin, 0.0f);
    EXPECT_FLOAT_EQ(a.xmax, 0.0f);
    EXPECT_FLOAT_EQ(a.ymax, 0.0f);
    EXPECT_FLOAT_EQ(a.zmax, 0.0f);
}

TEST(SlabInitTest, DefaultConstructExtents) {
    // Constructed as expected
    Slab a(-1.0f, -2.0f, -3.0f, 1.0f, 2.0f, 3.0f);
    EXPECT_FLOAT_EQ(a.xmin, -1.0f);
    EXPECT_FLOAT_EQ(a.ymin, -2.0f);
    EXPECT_FLOAT_EQ(a.zmin, -3.0f);
    EXPECT_FLOAT_EQ(a.xmax,  1.0f);
    EXPECT_FLOAT_EQ(a.ymax,  2.0f);
    EXPECT_FLOAT_EQ(a.zmax,  3.0f);
    // Make sure we're fixing min/max bounds
    Slab b(1.0f, 2.0f, 3.0f, -1.0f, -2.0f, -3.0f);
    EXPECT_FLOAT_EQ(b.xmin, -1.0f);
    EXPECT_FLOAT_EQ(b.ymin, -2.0f);
    EXPECT_FLOAT_EQ(b.zmin, -3.0f);
    EXPECT_FLOAT_EQ(b.xmax,  1.0f);
    EXPECT_FLOAT_EQ(b.ymax,  2.0f);
    EXPECT_FLOAT_EQ(b.zmax,  3.0f);
}

TEST(SlabInitTest, DefaultConstructMinAndSize) {
    Slab a(-1.0f, -2.0f, -3.0f, 5.0f);
    EXPECT_FLOAT_EQ(a.xmin, -1.0f);
    EXPECT_FLOAT_EQ(a.ymin, -2.0f);
    EXPECT_FLOAT_EQ(a.zmin, -3.0f);
    EXPECT_FLOAT_EQ(a.xmax, -1.0f + 5.0f);
    EXPECT_FLOAT_EQ(a.ymax, -2.0f + 5.0f);
    EXPECT_FLOAT_EQ(a.zmax, -3.0f + 5.0f);
}

TEST(SlabInitTest, CorrectMinMax) {
    // Manually initializing elements so we are sure to test
    // the direct call to correctMinMax() and not the constructor
    Slab a;

    // Initialized as expected
    a.xmin = -1.0f; a.ymin = -2.0f; a.zmin = -3.0f;
    a.xmax =  1.0f; a.ymax =  2.0f; a.zmax =  3.0f;
    a.correctMinMax();
    EXPECT_FLOAT_EQ(a.xmin, -1.0f);
    EXPECT_FLOAT_EQ(a.ymin, -2.0f);
    EXPECT_FLOAT_EQ(a.zmin, -3.0f);
    EXPECT_FLOAT_EQ(a.xmax,  1.0f);
    EXPECT_FLOAT_EQ(a.ymax,  2.0f);
    EXPECT_FLOAT_EQ(a.zmax,  3.0f);
    // Initialized with max values as min, and vice versa
    a.xmin =  1.0f; a.ymin =  2.0f; a.zmin =  3.0f;
    a.xmax = -1.0f; a.ymax = -2.0f; a.zmax = -3.0f;
    a.correctMinMax();
    EXPECT_FLOAT_EQ(a.xmin, -1.0f);
    EXPECT_FLOAT_EQ(a.ymin, -2.0f);
    EXPECT_FLOAT_EQ(a.zmin, -3.0f);
    EXPECT_FLOAT_EQ(a.xmax,  1.0f);
    EXPECT_FLOAT_EQ(a.ymax,  2.0f);
    EXPECT_FLOAT_EQ(a.zmax,  3.0f);
}

TEST(SlabDimTest, ElementDims) {
    // Initialized as expected
    Slab a(-1.0f, -2.0f, -3.0f, 1.0f, 2.0f, 3.0f);
    EXPECT_FLOAT_EQ(a.xdim(), 2.0f);
    EXPECT_FLOAT_EQ(a.ydim(), 4.0f);
    EXPECT_FLOAT_EQ(a.zdim(), 6.0f);
    // Make sure we're fixing min/max bounds
    Slab b(1.0f, 2.0f, 3.0f, -1.0f, -2.0f, -3.0f);
    EXPECT_FLOAT_EQ(b.xdim(), 2.0f);
    EXPECT_FLOAT_EQ(b.ydim(), 4.0f);
    EXPECT_FLOAT_EQ(b.zdim(), 6.0f);
}

TEST(SlabDimTest, MaxDim) {
    Slab a(-1.0f, -2.0f, -3.0f, 1.0f, 2.0f, 3.0f);
    EXPECT_FLOAT_EQ(a.maxdim(), 6.0f);
    Slab b(1.0f, 2.0f, 3.0f, -1.0f, -2.0f, -3.0f);
    EXPECT_FLOAT_EQ(b.maxdim(), 6.0f);
}

TEST(SlabMidpointTest, ElementMidpoint) {
    Slab a(3.0f, 7.0f, 5.0f, 9.0f, 10.0f, 25.0f);
    EXPECT_FLOAT_EQ(a.xmid(), (3.0f +  9.0f) * 0.5f);
    EXPECT_FLOAT_EQ(a.ymid(), (7.0f + 10.0f) * 0.5f);
    EXPECT_FLOAT_EQ(a.zmid(), (5.0f + 25.0f) * 0.5f);
}

TEST(SlabVolumeTest, TotalVolume) {
    // Initialized as expected
    Slab a(-1.0f, -2.0f, -3.0f, 1.0f, 2.0f, 3.0f);
    EXPECT_FLOAT_EQ(a.volume(), 48.0f);
    // Make sure we're fixing min/max bounds
    Slab b(1.0f, 2.0f, 3.0f, -1.0f, -2.0f, -3.0f);
    EXPECT_FLOAT_EQ(b.volume(), 48.0f);
}

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

