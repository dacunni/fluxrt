#include <algorithm>
#include <numeric>
#include <functional>
#include <iostream>

#include "trianglemeshoctree.h"
#include "trianglemesh.h"
#include "slab.h"

TriangleMeshOctree::TriangleMeshOctree(TriangleMesh & mesh)
    : mesh(mesh)
{

}

void TriangleMeshOctree::build()
{
    nodes.clear();
    nodes.reserve(256);

    auto rootIndex = addNode(0);

    Slab bounds = boundingBox(mesh.vertices);

    // Create a list of unique triangle indices
    std::vector<uint32_t> tris;
    tris.resize(mesh.numTriangles());
    std::iota(tris.begin(), tris.end(), 0u);

    triangles.reserve(mesh.numTriangles());
    buildNode(rootIndex, tris, bounds);
}

void TriangleMeshOctree::buildNode(uint32_t nodeIndex,
                                   const std::vector<uint32_t> & tris,
                                   const Slab & bounds)
{
    auto first = tris.begin();
    auto last = tris.end();

    // Note: We are not using a reference to the node, because
    //       recursive calls may reallocate the array.
    Node node = nodes[nodeIndex];

    if(uint32_t(last - first) <= buildCutOffNumTriangles
       || node.level >= buildMaxLevel) {
        node.firstTriangle = uint32_t(triangles.size());
        node.numTriangles = uint32_t(last - first);
        std::copy(first, last, std::back_inserter(triangles));
        nodes[nodeIndex] = node;
        return;
    }

    // Convenience constants
    const float xmin = bounds.xmin, xmax = bounds.xmax;
    const float ymin = bounds.ymin, ymax = bounds.ymax;
    const float zmin = bounds.zmin, zmax = bounds.zmax;
    // Find split planes
    const float xmid = bounds.xmid(), ymid = bounds.ymid(), zmid = bounds.zmid();

    // Helper functions for partitioning
    auto evalForVerts = [&](uint32_t ti, std::function<bool(const Position3 &)> p) {
        return p(mesh.triangleVertex(ti, 0)) ||
               p(mesh.triangleVertex(ti, 1)) ||
               p(mesh.triangleVertex(ti, 2));
    };

    auto isInXLow  = [&](uint32_t ti) { return evalForVerts(ti, [&](const Position3 & p) { return p.x < xmid; }); };
    auto isInXHigh = [&](uint32_t ti) { return evalForVerts(ti, [&](const Position3 & p) { return p.x > xmid; }); };
    auto isInYLow  = [&](uint32_t ti) { return evalForVerts(ti, [&](const Position3 & p) { return p.y < ymid; }); };
    auto isInYHigh = [&](uint32_t ti) { return evalForVerts(ti, [&](const Position3 & p) { return p.y > ymid; }); };
    auto isInZLow  = [&](uint32_t ti) { return evalForVerts(ti, [&](const Position3 & p) { return p.z < zmid; }); };
    auto isInZHigh = [&](uint32_t ti) { return evalForVerts(ti, [&](const Position3 & p) { return p.z > zmid; }); };

    // Bin in  X
    std::vector<uint32_t> L, H;
    std::copy_if(first, last, std::back_inserter(L), isInXLow);
    std::copy_if(first, last, std::back_inserter(H), isInXHigh);

    // Bin in  Y
    std::vector<uint32_t> LL, LH, HL, HH;
    std::copy_if(L.begin(), L.end(), std::back_inserter(LL), isInYLow);
    std::copy_if(L.begin(), L.end(), std::back_inserter(LH), isInYHigh);
    std::copy_if(H.begin(), H.end(), std::back_inserter(HL), isInYLow);
    std::copy_if(H.begin(), H.end(), std::back_inserter(HH), isInYHigh);

    // Bin in  Z
    std::vector<uint32_t> LLL, LHL, HLL, HHL, LLH, LHH, HLH, HHH;
    std::copy_if(LL.begin(), LL.end(), std::back_inserter(LLL), isInZLow);
    std::copy_if(LL.begin(), LL.end(), std::back_inserter(LLH), isInZHigh);
    std::copy_if(LH.begin(), LH.end(), std::back_inserter(LHL), isInZLow);
    std::copy_if(LH.begin(), LH.end(), std::back_inserter(LHH), isInZHigh);
    std::copy_if(HL.begin(), HL.end(), std::back_inserter(HLL), isInZLow);
    std::copy_if(HL.begin(), HL.end(), std::back_inserter(HLH), isInZHigh);
    std::copy_if(HH.begin(), HH.end(), std::back_inserter(HHL), isInZLow);
    std::copy_if(HH.begin(), HH.end(), std::back_inserter(HHH), isInZHigh);

    // Create child nodes
    buildChild(node, 0, LLL, Slab(xmin, ymin, zmin, xmid, ymid, zmid));
    buildChild(node, 1, LLH, Slab(xmin, ymin, zmid, xmid, ymid, zmax));
    buildChild(node, 2, LHL, Slab(xmin, ymid, zmin, xmid, ymax, zmid));
    buildChild(node, 3, LHH, Slab(xmin, ymid, zmid, xmid, ymax, zmax));
    buildChild(node, 4, HLL, Slab(xmid, ymin, zmin, xmax, ymid, zmid));
    buildChild(node, 5, HLH, Slab(xmid, ymin, zmid, xmax, ymid, zmax));
    buildChild(node, 6, HHL, Slab(xmid, ymid, zmin, xmax, ymax, zmid));
    buildChild(node, 7, HHH, Slab(xmid, ymid, zmid, xmax, ymax, zmax));

    // Update the node in the array
    nodes[nodeIndex] = node;
}

void TriangleMeshOctree::buildChild(Node & node,
                                    unsigned int childIndex,
                                    const std::vector<uint32_t> & childTris,
                                    const Slab & childBounds)
{
    if(childTris.size() < 1)
        return;
    auto childNodeIndex = addNode(node.level + 1);
    node.children[childIndex] = childNodeIndex;
    buildNode(childNodeIndex, childTris, childBounds);
}

uint32_t TriangleMeshOctree::addNode(uint8_t level)
{
    nodes.emplace_back();
    nodes.back().level = level;
    return nodes.size() - 1;
}

void TriangleMeshOctree::printNodes() const
{
    printf("triangles mesh %u octree %u\n",
           (unsigned int) mesh.numTriangles(),
           (unsigned int) triangles.size());
    for(uint32_t i = 0; i < nodes.size(); ++i) {
        auto & node = nodes[i];
        printf("%3u : level %2u first %u count %u\n", i,
               (unsigned int) node.level, node.firstTriangle, node.numTriangles);
        if(node.numChildren() > 0) {
            printf("      children  ");
            for(uint32_t c = 0; c < 8; ++c) {
                printf("%u=%u%s", c, node.children[c], c < 7 ? ", " : "\n");
            }
        }
    }
}

bool TriangleMeshOctree::nodesCoverAllTriangles() const
{
    std::vector<uint8_t> claimed(mesh.numTriangles());
    std::fill(claimed.begin(), claimed.end(), uint8_t(0));
    for(auto & node : nodes) {
        for(uint32_t ti = 0; ti < node.numTriangles; ++ti) {
            claimed[triangles[node.firstTriangle + ti]] = uint8_t(1);
        }
    }
    return std::find(claimed.begin(), claimed.end(), uint8_t(0)) == claimed.end();
}

uint32_t TriangleMeshOctree::Node::numChildren() const
{
    // count non-zero child indices
    return std::count_if(children, children + 6,
                         [](uint32_t index) { return index > 0; });
}

