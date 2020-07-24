#include <gtest/gtest.h>
#include "Sphere.h"

namespace {

// ---------------------- Intersection Predicate Tests ------------------------

TEST(RaySphereIntersectTest, OnAxisRay_CenteredSphere_IntersectsTrue) {
    const float minDistance = 0.01f, maxDistance = 1000.0f;
    const Sphere obj(Position3(0, 0, 0), 0.6);
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

TEST(RaySphereIntersectTest, OnAxisCenteredRay_CenteredSphere_IntersectsTrue) {
    const float minDistance = 0.01f, maxDistance = 1000.0f;
    const Sphere obj(Position3(0, 0, 0), 0.6);
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

TEST(RaySphereIntersectTest, FarAxisParallelRay_CenteredSphere_IntersectsFalse) {
    const float minDistance = 0.01f, maxDistance = 1000.0f;
    const Sphere obj(Position3(0, 0, 0), 0.6);
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

TEST(RaySphereIntersectTest, OffCenterDiagonalRay_OffCenterSphere_IntersectsTrue) {
    const float minDistance = 0.01f, maxDistance = 1000.0f;
    EXPECT_TRUE(intersects(Ray(Position3(0, 0, 2), Direction3(1, 1, 1).normalized()),
                           Sphere(Position3(5, 5, 7), 0.6),
                           minDistance, maxDistance));
}

// ---------------------- Find Intersection Tests ------------------------

TEST(RaySphereFindIntersectionTest, OnAxisRay_CenteredSphere_FindsFirstIntersection) {
    const float minDistance = 0.01f;
    const Sphere obj(Position3(0, 0, 0), 0.6);
    RayIntersection isect;
    const float rayOffset = 10.0f;

    // Looking down +x
    EXPECT_TRUE(findIntersection(Ray(Position3(-rayOffset, 0, 0), Direction3(1, 0, 0)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, rayOffset - obj.radius);
    // Looking down -x
    EXPECT_TRUE(findIntersection(Ray(Position3(rayOffset, 0, 0), Direction3(-1, 0, 0)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, rayOffset - obj.radius);
    // Looking down +y
    EXPECT_TRUE(findIntersection(Ray(Position3(0, -rayOffset, 0), Direction3(0, 1, 0)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, rayOffset - obj.radius);
    // Looking down -y
    EXPECT_TRUE(findIntersection(Ray(Position3(0, rayOffset, 0), Direction3(0, -1, 0)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, rayOffset - obj.radius);
    // Looking down +z
    EXPECT_TRUE(findIntersection(Ray(Position3(0, 0, -rayOffset), Direction3(0, 0, 1)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, rayOffset - obj.radius);
    // Looking down -z
    EXPECT_TRUE(findIntersection(Ray(Position3(0, 0, rayOffset), Direction3(0, 0, -1)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, rayOffset - obj.radius);
}

TEST(RaySphereFindIntersectionTest, OnAxisCenteredRay_CenteredSphere_FindsSecondIntersection) {
    const float minDistance = 0.01f;
    const Sphere obj(Position3(0, 0, 0), 0.6);
    RayIntersection isect;

    // Looking down +x
    EXPECT_TRUE(findIntersection(Ray(Position3(0, 0, 0), Direction3(1, 0, 0)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, obj.radius);
    // Looking down -x
    EXPECT_TRUE(findIntersection(Ray(Position3(0, 0, 0), Direction3(-1, 0, 0)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, obj.radius);
    // Looking down +y
    EXPECT_TRUE(findIntersection(Ray(Position3(0, 0, 0), Direction3(0, 1, 0)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, obj.radius);
    // Looking down -y
    EXPECT_TRUE(findIntersection(Ray(Position3(0, 0, 0), Direction3(0, -1, 0)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, obj.radius);
    // Looking down +z
    EXPECT_TRUE(findIntersection(Ray(Position3(0, 0, 0), Direction3(0, 0, 1)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, obj.radius);
    // Looking down -z
    EXPECT_TRUE(findIntersection(Ray(Position3(0, 0, 0), Direction3(0, 0, -1)), obj, minDistance, isect));
    EXPECT_FLOAT_EQ(isect.distance, obj.radius);
}

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

