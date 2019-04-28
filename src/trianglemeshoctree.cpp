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

    auto rootIndex = addNode();

    // Create a list of triangles that have not been claimed yet.
    // Fill with records to help us sort and keep track of which
    // child node each triangle belongs to.
    auto numTriangles = mesh.numTriangles();
    std::vector<NodeSortRecord> records(numTriangles);
    uint32_t triangleIndex = 0;
    std::generate(records.begin(), records.end(),
                  [&triangleIndex]() { return NodeSortRecord{triangleIndex++, 0}; });

    Slab bounds = boundingBox(mesh.vertices);

    buildNode(rootIndex, records, records.begin(), records.end(), bounds);

    // Populate triangle indices using the order established when we built
    // the Node hierarchy.
    triangles.resize(numTriangles);
    std::transform(records.begin(), records.end(), triangles.begin(),
                   [](const NodeSortRecord & rec) { return rec.triangleIndex; });
}

void TriangleMeshOctree::buildNode(uint32_t nodeIndex,
                                   std::vector<NodeSortRecord> & records,
                                   std::vector<NodeSortRecord>::iterator first,
                                   std::vector<NodeSortRecord>::iterator last,
                                   const Slab & bounds)
{
    // Note: We are not using a reference to the node, because
    //       recursive calls may reallocate the array.
    Node node = nodes[nodeIndex];

    if(uint32_t(last - first) <= buildCutOffNumTriangles
       || node.level >= buildMaxLevel) {
        node.firstTriangle = uint32_t(first - records.begin());
        node.numTriangles = uint32_t(last - first);
        nodes[nodeIndex] = node;
        return;
    }

    // Find split planes
    const float xmid = bounds.xmid();
    const float ymid = bounds.ymid();
    const float zmid = bounds.zmid();

    // Helper functions for partitioning
    auto evalForVerts = [&](uint32_t ti, std::function<bool(const Position3 &)> p) {
        return p(mesh.triangleVertex(ti, 0)) && p(mesh.triangleVertex(ti, 1)) && p(mesh.triangleVertex(ti, 2));
    };

    auto isInXLow  = [&](const NodeSortRecord & rec) { return evalForVerts(rec.triangleIndex, [&](const Position3 & p) { return p.x < xmid; }); };
    auto isInXHigh = [&](const NodeSortRecord & rec) { return evalForVerts(rec.triangleIndex, [&](const Position3 & p) { return p.x > xmid; }); };
    auto isInYLow  = [&](const NodeSortRecord & rec) { return evalForVerts(rec.triangleIndex, [&](const Position3 & p) { return p.y < ymid; }); };
    auto isInYHigh = [&](const NodeSortRecord & rec) { return evalForVerts(rec.triangleIndex, [&](const Position3 & p) { return p.y > ymid; }); };
    auto isInZLow  = [&](const NodeSortRecord & rec) { return evalForVerts(rec.triangleIndex, [&](const Position3 & p) { return p.z < zmid; }); };
    auto isInZHigh = [&](const NodeSortRecord & rec) { return evalForVerts(rec.triangleIndex, [&](const Position3 & p) { return p.z > zmid; }); };

    // Adjust child indices so we can quickly walk them after we've sorted them
    // along each direction and filtered out those owned by this node.
    std::transform(first, last, first, [](NodeSortRecord rec) { rec.childNodeIndex = 0; return rec; });
    std::transform(first, last, first, [&](NodeSortRecord rec) { if(isInXHigh(rec)) { rec.childNodeIndex |= 0x4; } return rec; });
    std::transform(first, last, first, [&](NodeSortRecord rec) { if(isInYHigh(rec)) { rec.childNodeIndex |= 0x2; } return rec; });
    std::transform(first, last, first, [&](NodeSortRecord rec) { if(isInZHigh(rec)) { rec.childNodeIndex |= 0x1; } return rec; });

    // Range of unclaimed triangles
    auto unclaimed = last;
    auto div = first;

    // Partition along Z, moving unclaimed nodes to the end
    div       = std::stable_partition(first, unclaimed, isInZLow);
    unclaimed = std::stable_partition(div,   unclaimed, isInZHigh);

    // Partition along Y, moving unclaimed nodes to the end
    div       = std::stable_partition(first, unclaimed, isInYLow);
    unclaimed = std::stable_partition(div,   unclaimed, isInYHigh);

    // Partition along X, moving unclaimed nodes to the end
    div       = std::stable_partition(first, unclaimed, isInXLow);
    unclaimed = std::stable_partition(div,   unclaimed, isInXHigh);

    // This Node will own any triangles not exclusively in a subregion
    if(unclaimed != last) {
        node.firstTriangle = uint32_t(unclaimed - records.begin());
        node.numTriangles = uint32_t(last - unclaimed);
    }

    // Create child nodes
    if(first != unclaimed) {
        auto childFirst = first;

        while(childFirst != unclaimed) {
            auto childLast = std::find_if(childFirst, unclaimed, [&](const NodeSortRecord & rec) {
                                          return rec.childNodeIndex != childFirst->childNodeIndex; });
            if(childLast - childFirst > 0) {
                auto childIndex = addNode();
                auto & childNode = nodes[childIndex];
                childNode.level = node.level + 1;
                node.children[childFirst->childNodeIndex] = childIndex;
                auto childBounds = bounds;
                if(childFirst->childNodeIndex & 0x4)
                    childBounds.xmin = xmid;
                else
                    childBounds.xmax = xmid;
                if(childFirst->childNodeIndex & 0x2)
                    childBounds.ymin = ymid;
                else
                    childBounds.ymax = ymid;
                if(childFirst->childNodeIndex & 0x1)
                    childBounds.zmin = zmid;
                else
                    childBounds.zmax = zmid;
                buildNode(childIndex, records, childFirst, childLast, childBounds);
            }
            childFirst = childLast;
        }
    }

    // Update the node in the array
    nodes[nodeIndex] = node;
}

uint32_t TriangleMeshOctree::addNode()
{
    nodes.emplace_back();
    return nodes.size() - 1;
}

void TriangleMeshOctree::printNodes() const
{
    for(uint32_t i = 0; i < nodes.size(); ++i) {
        auto & node = nodes[i];
        printf("%3u : level %2u first %u count %u\n", i, node.level, node.firstTriangle, node.numTriangles);
        printf("      children  ");
        for(uint32_t c = 0; c < 6; ++c) {
            printf("%u=%u%s", c, node.children[c], c < 5 ? ", " : "\n");
        }
    }
}

bool TriangleMeshOctree::nodesCoverAllTriangles() const
{
    std::vector<uint8_t> claimed(triangles.size());
    std::fill(claimed.begin(), claimed.end(), uint8_t(0));
    for(auto & node : nodes) {
        std::fill(&claimed[node.firstTriangle],
                  &claimed[node.firstTriangle + node.numTriangles], uint8_t(1));
    }
    return std::find(claimed.begin(), claimed.end(), uint8_t(0)) == claimed.end();
}

uint32_t TriangleMeshOctree::Node::numChildren() const
{
    // count non-zero child indices
    return std::count_if(children, children + 6,
                         [](uint32_t index) { return index > 0; });
}

