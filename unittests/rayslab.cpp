#include <gtest/gtest.h>
#include "slab.h"

namespace {

// ---------------------- Intersection Predicate Tests ------------------------

TEST(RaySlabIntersectTest, OnAxisRay_CenteredSlab_IntersectsTrue) {
    const float minDistance = 0.01f, maxDistance = 1000.0f;
    Slab obj(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f);
    // Looking down +x
    EXPECT_TRUE(intersects(Ray(Position3(-10, 0, 0), Direction3(1, 0, 0)), obj, minDistance, maxDistance));
    // Looking down -x
    EXPECT_TRUE(intersects(Ray(Position3(10, 0, 0), Direction3(-1, 0, 0)), obj, minDistance, maxDistance));
    // Looking down +y
    EXPECT_TRUE(intersects(Ray(Position3(0, -10, 0), Direction3(0, 1, 0)), obj, minDistance, maxDistance));
    // Looking down -y
    EXPECT_TRUE(intersects(Ray(Position3(0, 10, 0), Direction3(0, -1, 0)), obj, minDistance, maxDistance));
    // Looking down +z
    EXPECT_TRUE(intersects(Ray(Position3(0, 0, -10), Direction3(0, 0, 1)), obj, minDistance, maxDistance));
    // Looking down -z
    EXPECT_TRUE(intersects(Ray(Position3(0, 0, 10), Direction3(0, 0, -1)), obj, minDistance, maxDistance));
}

TEST(RaySlabIntersectTest, OnAxisCenteredRay_CenteredSlab_IntersectsTrue) {
    const float minDistance = 0.01f, maxDistance = 1000.0f;
    Slab obj(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f);
    // Looking down +x
    EXPECT_TRUE(intersects(Ray(Position3(0, 0, 0), Direction3(1, 0, 0)), obj, minDistance, maxDistance));
    // Looking down -x
    EXPECT_TRUE(intersects(Ray(Position3(0, 0, 0), Direction3(-1, 0, 0)), obj, minDistance, maxDistance));
    // Looking down +y
    EXPECT_TRUE(intersects(Ray(Position3(0, 0, 0), Direction3(0, 1, 0)), obj, minDistance, maxDistance));
    // Looking down -y
    EXPECT_TRUE(intersects(Ray(Position3(0, 0, 0), Direction3(0, -1, 0)), obj, minDistance, maxDistance));
    // Looking down +z
    EXPECT_TRUE(intersects(Ray(Position3(0, 0, 0), Direction3(0, 0, 1)), obj, minDistance, maxDistance));
    // Looking down -z
    EXPECT_TRUE(intersects(Ray(Position3(0, 0, 0), Direction3(0, 0, -1)), obj, minDistance, maxDistance));
}

TEST(RaySlabIntersectTest, FarAxisParallelRay_CenteredSlab_IntersectsFalse) {
    const float minDistance = 0.01f, maxDistance = 1000.0f;
    Slab obj(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f);
    // Looking down +x
    EXPECT_FALSE(intersects(Ray(Position3(-10, 2, 0), Direction3(1, 0, 0)), obj, minDistance, maxDistance));
    // Looking down -x
    EXPECT_FALSE(intersects(Ray(Position3(10, 2, 0), Direction3(-1, 0, 0)), obj, minDistance, maxDistance));
    // Looking down +y
    EXPECT_FALSE(intersects(Ray(Position3(2, -10, 0), Direction3(0, 1, 0)), obj, minDistance, maxDistance));
    // Looking down -y
    EXPECT_FALSE(intersects(Ray(Position3(2, 10, 0), Direction3(0, -1, 0)), obj, minDistance, maxDistance));
    // Looking down +z
    EXPECT_FALSE(intersects(Ray(Position3(2, 0, -10), Direction3(0, 0, 1)), obj, minDistance, maxDistance));
    // Looking down -z
    EXPECT_FALSE(intersects(Ray(Position3(2, 0, 10), Direction3(0, 0, -1)), obj, minDistance, maxDistance));
}

TEST(RaySlabIntersectTest, OffCenterDiagonalRay_OffCenterSlab_IntersectsTrue) {
    const float minDistance = 0.01f, maxDistance = 1000.0f;
    EXPECT_TRUE(intersects(Ray(Position3(0, 0, 2), Direction3(1, 1, 1).normalized()),
                           Slab(4.0f, 4.0f, 4.0f, 6.0f, 6.0f, 6.0f),
                           minDistance, maxDistance));
}

// ---------------------- Find Intersection Tests ------------------------

TEST(RaySlabFindIntersectionTest, OnAxisCenteredRay_CenteredSlab_FindsFirstIntersection) {
    const float minDistance = 0.01f;
    const Slab obj(-0.6f, -0.6f, -0.6f, 0.6f, 0.6f, 0.6f);
    RayIntersection isect;

    // Looking down +x
    EXPECT_TRUE(findIntersection(Ray(Position3(0, 0, 0), Direction3(1, 0, 0)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, std::abs(0 - obj.xmin));
    // Looking down -x
    EXPECT_TRUE(findIntersection(Ray(Position3(0, 0, 0), Direction3(-1, 0, 0)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, std::abs(0 - obj.xmax));
    // Looking down +y
    EXPECT_TRUE(findIntersection(Ray(Position3(0, 0, 0), Direction3(0, 1, 0)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, std::abs(0 - obj.ymin));
    // Looking down -y
    EXPECT_TRUE(findIntersection(Ray(Position3(0, 0, 0), Direction3(0, -1, 0)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, std::abs(0 - obj.ymax));
    // Looking down +z
    EXPECT_TRUE(findIntersection(Ray(Position3(0, 0, 0), Direction3(0, 0, 1)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, std::abs(0 - obj.zmin));
    // Looking down -z
    EXPECT_TRUE(findIntersection(Ray(Position3(0, 0, 0), Direction3(0, 0, -1)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, std::abs(0 - obj.zmax));
}

TEST(RaySlabFindIntersectionTest, OnAxisRay_CenteredSlab_FindsFirstIntersection) {
    const float minDistance = 0.01f;
    const Slab obj(-0.6f, -0.6f, -0.6f, 0.6f, 0.6f, 0.6f);
    RayIntersection isect;
    const float rayOffset = 10.0f;

    // Looking down +x
    EXPECT_TRUE(findIntersection(Ray(Position3(-rayOffset, 0, 0), Direction3(1, 0, 0)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, std::abs(-rayOffset - obj.xmin));
    // Looking down -x
    EXPECT_TRUE(findIntersection(Ray(Position3(rayOffset, 0, 0), Direction3(-1, 0, 0)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, std::abs(rayOffset - obj.xmax));
    // Looking down +y
    EXPECT_TRUE(findIntersection(Ray(Position3(0, -rayOffset, 0), Direction3(0, 1, 0)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, std::abs(-rayOffset - obj.ymin));
    // Looking down -y
    EXPECT_TRUE(findIntersection(Ray(Position3(0, rayOffset, 0), Direction3(0, -1, 0)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, std::abs(rayOffset - obj.ymax));
    // Looking down +z
    EXPECT_TRUE(findIntersection(Ray(Position3(0, 0, -rayOffset), Direction3(0, 0, 1)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, std::abs(-rayOffset - obj.zmin));
    // Looking down -z
    EXPECT_TRUE(findIntersection(Ray(Position3(0, 0, rayOffset), Direction3(0, 0, -1)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, std::abs(rayOffset - obj.zmax));
}

TEST(RaySlabFindIntersectionTest, OnAxisRayWithNegativeZeros_CenteredSlab_FindsFirstIntersection) {
    const float minDistance = 0.01f;
    const Slab obj(-0.6f, -0.6f, -0.6f, 0.6f, 0.6f, 0.6f);
    RayIntersection isect;
    const float rayOffset = 10.0f;

    // Looking down +x
    EXPECT_TRUE(findIntersection(Ray(Position3(-rayOffset, 0, 0), Direction3(1, -0.0f, -0.0f)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, std::abs(-rayOffset - obj.xmin));
    // Looking down -x
    EXPECT_TRUE(findIntersection(Ray(Position3(rayOffset, 0, 0), Direction3(-1, -0.0f, -0.0f)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, std::abs(rayOffset - obj.xmax));
    // Looking down +y
    EXPECT_TRUE(findIntersection(Ray(Position3(0, -rayOffset, 0), Direction3(-0.0f, 1, -0.0f)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, std::abs(-rayOffset - obj.ymin));
    // Looking down -y
    EXPECT_TRUE(findIntersection(Ray(Position3(0, rayOffset, 0), Direction3(-0.0f, -1, -0.0f)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, std::abs(rayOffset - obj.ymax));
    // Looking down +z
    EXPECT_TRUE(findIntersection(Ray(Position3(0, 0, -rayOffset), Direction3(-0.0f, -0.0f, 1)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, std::abs(-rayOffset - obj.zmin));
    // Looking down -z
    EXPECT_TRUE(findIntersection(Ray(Position3(0, 0, rayOffset), Direction3(-0.0f, -0.0f, -1)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, std::abs(rayOffset - obj.zmax));
}

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

