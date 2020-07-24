#include <gtest/gtest.h>
#include "vectortypes.h"
#include "TriangleMeshOctree.h"

namespace {

// ---------------------- Octree Traversal Tests ------------------------

using child_index_t = TriangleMeshOctree::child_index_t;
using child_array_t = TriangleMeshOctree::child_array_t;

bool bitEqual(child_index_t bit, child_index_t v1, child_index_t v2) {
    return (v1 & bit) == (v2 & bit);
}

bool bitEqual(child_index_t bit, const child_array_t & arr1, const child_array_t & arr2) {
    for(child_index_t ci = 0; ci < TriangleMeshOctree::MAX_CHILDREN; ++ci) {
        if(!bitEqual(bit, arr1[ci], arr2[ci]))
           return false;
    }
    return true;
}

auto MAXCH = TriangleMeshOctree::MAX_CHILDREN;

auto LLL = TriangleMeshOctree::LLL;
auto LLH = TriangleMeshOctree::LLH;
auto LHL = TriangleMeshOctree::LHL;
auto LHH = TriangleMeshOctree::LHH;
auto HLL = TriangleMeshOctree::HLL;
auto HLH = TriangleMeshOctree::HLH;
auto HHL = TriangleMeshOctree::HHL;
auto HHH = TriangleMeshOctree::HHH;

TEST(RayTriangleMeshOctreeTraversal, OnAxisDirections_ProperChildTraversalOrder) {
    // For on-axis directions, it is ill-defined what the second and third direction traversals
    // should be. So we just test the first traversal direction, and pass LLL and HHH to match
    // the direction we care about in each test.

    child_array_t children = {};

    TriangleMeshOctree::childOrderForDirection(Direction3(1, 0, 0), children);
    EXPECT_TRUE(bitEqual(TriangleMeshOctree::XBIT, children, child_array_t{ LLL, LLL, LLL, LLL, HHH, HHH, HHH, HHH })); 

    TriangleMeshOctree::childOrderForDirection(Direction3(-1, 0, 0), children);
    EXPECT_TRUE(bitEqual(TriangleMeshOctree::XBIT, children, child_array_t{ HHH, HHH, HHH, HHH, LLL, LLL, LLL, LLL })); 

    TriangleMeshOctree::childOrderForDirection(Direction3(0, 1, 0), children);
    EXPECT_TRUE(bitEqual(TriangleMeshOctree::YBIT, children, child_array_t{ LLL, LLL, LLL, LLL, HHH, HHH, HHH, HHH })); 

    TriangleMeshOctree::childOrderForDirection(Direction3(0, -1, 0), children);
    EXPECT_TRUE(bitEqual(TriangleMeshOctree::YBIT, children, child_array_t{ HHH, HHH, HHH, HHH, LLL, LLL, LLL, LLL })); 

    TriangleMeshOctree::childOrderForDirection(Direction3(0, 0, 1), children);
    EXPECT_TRUE(bitEqual(TriangleMeshOctree::ZBIT, children, child_array_t{ LLL, LLL, LLL, LLL, HHH, HHH, HHH, HHH })); 

    TriangleMeshOctree::childOrderForDirection(Direction3(0, 0, -1), children);
    EXPECT_TRUE(bitEqual(TriangleMeshOctree::ZBIT, children, child_array_t{ HHH, HHH, HHH, HHH, LLL, LLL, LLL, LLL })); 
}

TEST(RayTriangleMeshOctreeTraversal, GeneralDirections_ProperChildTraversalOrder) {
    child_array_t children = {}, expected = {};

    // +X, +Y, +Z
    TriangleMeshOctree::childOrderForDirection(Direction3(1, 0.5, 0.1), children);
    {
    child_array_t expected{ LLL, LLH, LHL, LHH, HLL, HLH, HHL, HHH };
    EXPECT_TRUE(std::equal(children, &children[MAXCH], expected));
    }

    // +X, +Z, +Y
    TriangleMeshOctree::childOrderForDirection(Direction3(1, 0.1, 0.5), children);
    {
    child_array_t expected{ LLL, LHL, LLH, LHH, HLL, HHL, HLH, HHH };
    EXPECT_TRUE(std::equal(children, &children[MAXCH], expected));
    }

    // TODO - Add other orderings

}

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

