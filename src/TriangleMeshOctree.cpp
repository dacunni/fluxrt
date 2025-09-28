#include <algorithm>
#include <numeric>
#include <functional>
#include <iostream>
#include <cassert>
#include <stack>

#include "TriangleMeshOctree.h"
#include "TriangleMesh.h"
#include "Triangle.h"
#include "vectortypes.h"
#include "slab.h"

TriangleMeshOctree::TriangleMeshOctree(std::shared_ptr<TriangleMesh> & mesh)
    : mesh(mesh)
{
    transform = mesh->transform;
}

void TriangleMeshOctree::build()
{
    nodes.clear();
    nodes.reserve(256);

    auto rootIndex = addNode(0);

    Slab bounds = ::boundingBox(mesh->meshData->vertices);

    // Create a list of unique triangle indices
    std::vector<uint32_t> tris;
    tris.resize(mesh->numTriangles());
    std::iota(tris.begin(), tris.end(), 0u);

    triangles.reserve(mesh->numTriangles());
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
    node.bounds = bounds;

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
        return p(mesh->triangleVertex(ti, 0))
            || p(mesh->triangleVertex(ti, 1))
            || p(mesh->triangleVertex(ti, 2));
    };

    // FIXME: This is not sufficient to determine if a triangle intersects a bounding box. There is a WAR in findIntersection(),
    //        but the proper fix should be here, and the WAR may not catch all cases.
    auto isInXLow  = [&](uint32_t ti) { return evalForVerts(ti, [&](const Position3 & p) { return p.x <= xmid; }); };
    auto isInXHigh = [&](uint32_t ti) { return evalForVerts(ti, [&](const Position3 & p) { return p.x >= xmid; }); };
    auto isInYLow  = [&](uint32_t ti) { return evalForVerts(ti, [&](const Position3 & p) { return p.y <= ymid; }); };
    auto isInYHigh = [&](uint32_t ti) { return evalForVerts(ti, [&](const Position3 & p) { return p.y >= ymid; }); };
    auto isInZLow  = [&](uint32_t ti) { return evalForVerts(ti, [&](const Position3 & p) { return p.z <= zmid; }); };
    auto isInZHigh = [&](uint32_t ti) { return evalForVerts(ti, [&](const Position3 & p) { return p.z >= zmid; }); };

    // Bin in X
    std::vector<uint32_t> L, H;
    std::copy_if(first, last, std::back_inserter(L), isInXLow);
    std::copy_if(first, last, std::back_inserter(H), isInXHigh);

    // Bin in Y
    std::vector<uint32_t> LL, LH, HL, HH;
    std::copy_if(L.begin(), L.end(), std::back_inserter(LL), isInYLow);
    std::copy_if(L.begin(), L.end(), std::back_inserter(LH), isInYHigh);
    std::copy_if(H.begin(), H.end(), std::back_inserter(HL), isInYLow);
    std::copy_if(H.begin(), H.end(), std::back_inserter(HH), isInYHigh);

    // Bin in Z
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
    buildChild(node, TriangleMeshOctree::LLL, LLL, Slab(xmin, ymin, zmin, xmid, ymid, zmid));
    buildChild(node, TriangleMeshOctree::LLH, LLH, Slab(xmin, ymin, zmid, xmid, ymid, zmax));
    buildChild(node, TriangleMeshOctree::LHL, LHL, Slab(xmin, ymid, zmin, xmid, ymax, zmid));
    buildChild(node, TriangleMeshOctree::LHH, LHH, Slab(xmin, ymid, zmid, xmid, ymax, zmax));
    buildChild(node, TriangleMeshOctree::HLL, HLL, Slab(xmid, ymin, zmin, xmax, ymid, zmid));
    buildChild(node, TriangleMeshOctree::HLH, HLH, Slab(xmid, ymin, zmid, xmax, ymid, zmax));
    buildChild(node, TriangleMeshOctree::HHL, HHL, Slab(xmid, ymid, zmin, xmax, ymax, zmid));
    buildChild(node, TriangleMeshOctree::HHH, HHH, Slab(xmid, ymid, zmid, xmax, ymax, zmax));

    // Count non-zero child indices to determine number of children
    node.numChildren = std::count_if(node.children, node.children + MAX_CHILDREN, [](uint32_t index) { return index != NO_CHILD; });

    // Update the node in the array
    nodes[nodeIndex] = node;
}

void TriangleMeshOctree::buildChild(Node & node,
                                    child_index_t childIndex,
                                    const std::vector<uint32_t> & childTris,
                                    const Slab & childBounds)
{
    if(childTris.empty())
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
           (unsigned int) mesh->numTriangles(),
           (unsigned int) triangles.size());
    for(uint32_t i = 0; i < nodes.size(); ++i) {
        auto & node = nodes[i];
        printf("%3u : level %2u first %u count %u\n", i,
               (unsigned int) node.level, node.firstTriangle, node.numTriangles);
        if(node.numChildren > 0) {
            printf("      children  ");
            for(uint32_t c = 0; c < MAX_CHILDREN; ++c) {
                printf("%u=%u%s", c, node.children[c], c < 7 ? ", " : "\n");
            }
        }
    }
}

bool TriangleMeshOctree::nodesCoverAllTriangles() const
{
    std::vector<uint8_t> claimed(mesh->numTriangles());
    std::fill(claimed.begin(), claimed.end(), uint8_t(0));
    for(auto & node : nodes) {
        for(uint32_t ti = 0; ti < node.numTriangles; ++ti) {
            claimed[triangles[node.firstTriangle + ti]] = uint8_t(1);
        }
    }
    return std::find(claimed.begin(), claimed.end(), uint8_t(0)) == claimed.end();
}

void TriangleMeshOctree::childOrderForDirection(const Direction3 & d, child_array_t indices)
{
    // Direction sign sense (0: negative, 1: positive)
    child_index_t xsense = d.x < 0 ? 0u : 1u;
    child_index_t ysense = d.y < 0 ? 0u : 1u;
    child_index_t zsense = d.z < 0 ? 0u : 1u;

    // Direction magnitudes
    float xmag = std::abs(d.x);
    float ymag = std::abs(d.y);
    float zmag = std::abs(d.z);

    child_index_t xstep = 1, ystep = 1, zstep = 1;

    if(xmag > ymag) xstep *= 2; else ystep *= 2;
    if(xmag > zmag) xstep *= 2; else zstep *= 2;
    if(ymag > zmag) ystep *= 2; else zstep *= 2;

    for(child_index_t ci = 0; ci < MAX_CHILDREN; ++ci) {
        indices[ci] = 0;
        if( ((ci / xstep) & 0x1) == xsense )
            indices[ci] |= XBIT;
        if( ((ci / ystep) & 0x1) == ysense )
            indices[ci] |= YBIT;
        if( ((ci / zstep) & 0x1) == zsense )
            indices[ci] |= ZBIT;
    }
}

void TriangleMeshOctree::reverseChildOrderForDirection(const Direction3 & d, child_array_t indices)
{
    childOrderForDirection(d, indices);
    std::reverse(&indices[0], &indices[MAX_CHILDREN]);
}

const char * TriangleMeshOctree::octantString(child_index_t childIndex)
{
    switch(childIndex) {
        case LLL: return "LLL";
        case LLH: return "LLH";
        case LHL: return "LHL";
        case LHH: return "LHH";
        case HLL: return "HLL";
        case HLH: return "HLH";
        case HHL: return "HHL";
        case HHH: return "HHH";
        default:  return "???";
    }
}

void TriangleMeshOctree::printChildOrder(child_array_t & indices)
{
    for(auto childIndex : indices) {
        printf("%u (%s)  ", childIndex, octantString(childIndex));
    }
    printf("\n");
}

// Ray intersection

bool TriangleMeshOctree::intersectsNode(const Ray & ray, float minDistance, float maxDistance,
                                        const TriangleMeshOctree::child_array_t & childOrder,
                                        uint32_t nodeIndex) const
{
    auto & node = nodes[nodeIndex];

    if(!node.bounds.intersects(ray, minDistance, maxDistance))
       return false;

    if(node.numTriangles > 0) {
        for(uint32_t ti = 0; ti < node.numTriangles; ++ti) {
            auto tri = triangles[node.firstTriangle + ti];
            if(intersectsTriangle(ray,
                                  mesh->triangleVertex(tri, 0), mesh->triangleVertex(tri, 1), mesh->triangleVertex(tri, 2),
                                  minDistance, maxDistance)) {
                return true;
            }
        }
    }

    if(node.numChildren > 0) {
        for(auto childIndex : childOrder) {
            auto childNode = node.children[childIndex];
            if(childNode == TriangleMeshOctree::NO_CHILD)
                continue; // empty child cell
            if(intersectsNode(ray, minDistance, maxDistance, childOrder, childNode)) {
                return true;
            }
        }
    }

    return false;
}

bool TriangleMeshOctree::intersects(const Ray & ray, float minDistance, float maxDistance) const
{
    TriangleMeshOctree::child_array_t childOrder = {};
    TriangleMeshOctree::childOrderForDirection(ray.direction, childOrder);

    return intersectsNode(ray, minDistance, maxDistance, childOrder, 0);
}

bool TriangleMeshOctree::findIntersectionNode(const Ray & ray, float minDistance,
                                              uint32_t & bestTriangle, float & bestDistance,
                                              const TriangleMeshOctree::child_array_t & childOrder,
                                              uint32_t nodeIndex,
                                              uint32_t level) const
{
    assert(level < buildMaxLevel);
    assert(nodeIndex < nodes.size());
    auto & node = nodes[nodeIndex];
    bool hit = false;

    if(!node.bounds.intersects(ray, minDistance, std::numeric_limits<float>::max()))
       return false;
    
    hit |= findIntersectionNodeTriangles(
                ray, minDistance, bestTriangle, bestDistance,
                nodeIndex);

    if(node.numChildren > 0) {
        for(auto childIndex : childOrder) {
            auto childNode = node.children[childIndex];
            if(childNode == TriangleMeshOctree::NO_CHILD)
                continue; // empty child cell
            if(findIntersectionNode(ray, minDistance, bestTriangle, bestDistance, childOrder, childNode, level + 1)) {
                // FIXME[WAR]: We have a bug in the octree building logic that includes triangles in cells
                //             that do not actually contain them. This leads to incorrectly determining the first
                //             hit in the wrong cell sometimes. By iterating over all children, we make sure to
                //             find the true nearest hit.  The right fix would be to build the tree correctly in
                //             the first place.
                hit = true; // WAR
                //return true;
            }
        }
    }

    return hit;
}

bool TriangleMeshOctree::findIntersectionNodeTriangles(
    const Ray & ray,
    float minDistance,
    uint32_t & bestTriangle,
    float & bestDistance,
    uint32_t nodeIndex) const
{
    auto & node = nodes[nodeIndex];
    bool hit = false;

    if(node.numTriangles > 0) {
        float t = FLT_MAX;
        assert(node.firstTriangle + node.numTriangles - 1 < triangles.size());
        for(uint32_t ti = 0; ti < node.numTriangles; ++ti) {
            auto tri = triangles[node.firstTriangle + ti];
            if(intersectsTriangle(ray,
                                  mesh->triangleVertex(tri, 0),
                                  mesh->triangleVertex(tri, 1),
                                  mesh->triangleVertex(tri, 2),
                                  minDistance, std::numeric_limits<float>::max(), &t)) {
                if(t < bestDistance) {
                    bestTriangle = tri;
                    bestDistance = t;
                    hit = true;
                }
            }
        }
    }

    return hit;
}

bool TriangleMeshOctree::findIntersection(const Ray & ray, float minDistance,
                                          RayIntersection & intersection) const
{
    TriangleMeshOctree::child_array_t childOrder = {};
    std::stack<uint32_t> nodesToCheck;
    uint32_t bestTriangle = 0;
    float bestDistance = FLT_MAX;
    bool hit = false;

    // Get the reverse child order because we are setting up a depth-first
    // search using a stack
    TriangleMeshOctree::reverseChildOrderForDirection(ray.direction, childOrder);

    // Start at the root node
    nodesToCheck.push(0);

    while(!nodesToCheck.empty()) {
        uint32_t nodeIndex = nodesToCheck.top();
        nodesToCheck.pop();
        auto & node = nodes[nodeIndex];

        // Check intersection with bounding box for node
        if(!node.bounds.intersects(ray, minDistance, std::numeric_limits<float>::max()))
           continue;
        
        // Check intersection with triangles in the node
        hit |= findIntersectionNodeTriangles(ray, minDistance, bestTriangle, bestDistance, nodeIndex);

        // Push children onto the stack in reverse order so the resulting
        // traversal is depth-first from near to far along the ray.
        if(node.numChildren > 0) {
            for(auto childIndex : childOrder) {
                auto childNode = node.children[childIndex];
                if(childNode == TriangleMeshOctree::NO_CHILD)
                    continue; // empty child cell
                nodesToCheck.push(childNode);
            }
        }
    }

    if(!hit)
        return false;

    mesh->fillTriangleMeshIntersection(ray, bestTriangle, bestDistance, intersection);

    return true;
}

Slab TriangleMeshOctree::boundingBox()
{
    if(!nodes.empty()) {
        return nodes[0].bounds;
    }
    else {
        return Slab();
    }
}

