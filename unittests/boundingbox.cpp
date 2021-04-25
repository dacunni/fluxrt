#include <gtest/gtest.h>
#include "slab.h"
#include "Sphere.h"
#include "TriangleMesh.h"
#include "TriangleMeshOctree.h"
#include "DiskLight.h"

namespace {

TEST(BoundingBoxExtentsTest, SlabBoundingBoxExtents) {
    Slab a(-3.0f, -4.0, 5.2, 4.5, 7.6, 9.8);
    Slab bounds = a.boundingBox();
    EXPECT_FLOAT_EQ(a.xmin, bounds.xmin);
    EXPECT_FLOAT_EQ(a.ymin, bounds.ymin);
    EXPECT_FLOAT_EQ(a.zmin, bounds.zmin);
    EXPECT_FLOAT_EQ(a.xmax, bounds.xmax);
    EXPECT_FLOAT_EQ(a.ymax, bounds.ymax);
    EXPECT_FLOAT_EQ(a.zmax, bounds.zmax);
}

TEST(BoundingBoxExtentsTest, SphereBoundingBoxExtents) {
    Sphere a(Position3(3.0f, -4.0f, 5.0f), 2.0f);
    Slab bounds = a.boundingBox();
    auto c = a.center;
    auto r = a.radius;
    EXPECT_FLOAT_EQ(c.x - r, bounds.xmin);
    EXPECT_FLOAT_EQ(c.y - r, bounds.ymin);
    EXPECT_FLOAT_EQ(c.z - r, bounds.zmin);
    EXPECT_FLOAT_EQ(c.x + r, bounds.xmax);
    EXPECT_FLOAT_EQ(c.y + r, bounds.ymax);
    EXPECT_FLOAT_EQ(c.z + r, bounds.zmax);
}

TEST(BoundingBoxExtentsTest, TriangleMeshBoundingBoxExtents) {
    TriangleMesh mesh;
    mesh.vertices = {
        Position3{ -1.0f, 0.0f,  0.0f },
        Position3{  0.0f,  2.0f, 3.0f },
        Position3{  0.0f, -4.0f, 5.0f },
        Position3{  7.0f, -4.0f, 5.0f },
    };
    mesh.bounds = ::boundingBox(mesh.vertices);
    Slab bounds = mesh.boundingBox();
    EXPECT_FLOAT_EQ(bounds.xmin, -1.0f);
    EXPECT_FLOAT_EQ(bounds.ymin, -4.0f);
    EXPECT_FLOAT_EQ(bounds.zmin, 0.0f);
    EXPECT_FLOAT_EQ(bounds.xmax, 7.0f);
    EXPECT_FLOAT_EQ(bounds.ymax, 2.0f);
    EXPECT_FLOAT_EQ(bounds.zmax, 5.0f);
}


TEST(BoundingBoxExtentsTest, TriangleMeshOctreeBoundingBoxExtents) {
    auto mesh = std::make_shared<TriangleMesh>();
    mesh->vertices = {
        Position3{ -1.0f, 0.0f,  0.0f },
        Position3{  0.0f,  2.0f, 3.0f },
        Position3{  0.0f, -4.0f, 5.0f },
        Position3{  7.0f, -4.0f, 5.0f },
    };
    mesh->bounds = ::boundingBox(mesh->vertices);

    TriangleMeshOctree octree(mesh);
    octree.build();
    Slab bounds = octree.boundingBox();
    EXPECT_FLOAT_EQ(bounds.xmin, -1.0f);
    EXPECT_FLOAT_EQ(bounds.ymin, -4.0f);
    EXPECT_FLOAT_EQ(bounds.zmin, 0.0f);
    EXPECT_FLOAT_EQ(bounds.xmax, 7.0f);
    EXPECT_FLOAT_EQ(bounds.ymax, 2.0f);
    EXPECT_FLOAT_EQ(bounds.zmax, 5.0f);
}

TEST(BoundingBoxExtentsTest, DiskLightBoundingBoxExtents) {
    Position3 p(3.0f, -4.0f, 5.0f);
    float r = 2.0f;
    {
        DiskLight a(p, Direction3(1.0f, 0.0f, 0.0f), r);
        Slab bounds = a.boundingBox();
        EXPECT_FLOAT_EQ(bounds.xmin,  3.0f);
        EXPECT_FLOAT_EQ(bounds.ymin, -6.0f);
        EXPECT_FLOAT_EQ(bounds.zmin,  3.0f);
        EXPECT_FLOAT_EQ(bounds.xmax,  3.0f);
        EXPECT_FLOAT_EQ(bounds.ymax, -2.0f);
        EXPECT_FLOAT_EQ(bounds.zmax,  7.0f);
    }
    {
        DiskLight a(p, Direction3(0.0f, 1.0f, 0.0f), r);
        Slab bounds = a.boundingBox();
        EXPECT_FLOAT_EQ(bounds.xmin,  1.0f);
        EXPECT_FLOAT_EQ(bounds.ymin, -4.0f);
        EXPECT_FLOAT_EQ(bounds.zmin,  3.0f);
        EXPECT_FLOAT_EQ(bounds.xmax,  5.0f);
        EXPECT_FLOAT_EQ(bounds.ymax, -4.0f);
        EXPECT_FLOAT_EQ(bounds.zmax,  7.0f);
    }
    {
        DiskLight a(p, Direction3(0.0f, 0.0f, -1.0f), r);
        Slab bounds = a.boundingBox();
        EXPECT_FLOAT_EQ(bounds.xmin,  1.0f);
        EXPECT_FLOAT_EQ(bounds.ymin, -6.0f);
        EXPECT_FLOAT_EQ(bounds.zmin,  5.0f);
        EXPECT_FLOAT_EQ(bounds.xmax,  5.0f);
        EXPECT_FLOAT_EQ(bounds.ymax, -2.0f);
        EXPECT_FLOAT_EQ(bounds.zmax,  5.0f);
    }
    {
        DiskLight a(p, Direction3(1.0f, 1.0f, 0.0f).normalized(), r);
        Slab bounds = a.boundingBox();
        float s = 1.0f / std::sqrt(2.0f);
        EXPECT_FLOAT_EQ(bounds.xmin, p.x - s * r);
        EXPECT_FLOAT_EQ(bounds.ymin, p.y - s * r);
        EXPECT_FLOAT_EQ(bounds.zmin, p.z - r);
        EXPECT_FLOAT_EQ(bounds.xmax, p.x + s * r);
        EXPECT_FLOAT_EQ(bounds.ymax, p.y + s * r);
        EXPECT_FLOAT_EQ(bounds.zmax, p.z + r);
    }
}

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

