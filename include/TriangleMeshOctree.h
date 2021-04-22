#ifndef __TRIANGLE_MESH_OCTREE_H__
#define __TRIANGLE_MESH_OCTREE_H__

#include <vector>
#include <cstdint>

#include "traceable.h"
#include "slab.h"

struct Ray;
struct RayIntersection;

struct TriangleMesh;
struct Direction3;

struct TriangleMeshOctree : public Traceable
{
    using child_index_t = unsigned int;
    static const child_index_t MAX_CHILDREN = 8;
    static const child_index_t NO_CHILD = 0;
    using child_array_t = child_index_t[MAX_CHILDREN];

    TriangleMeshOctree(TriangleMesh & mesh);
    ~TriangleMeshOctree() = default;

    // Ray intersection implementation
    virtual bool intersects(const Ray & ray, float minDistance, float maxDistance) const override;
    virtual bool findIntersection(const Ray & ray, float minDistance, RayIntersection & intersection) const override;

    bool intersectsNode(const Ray & ray, float minDistance, float maxDistance,
                        const TriangleMeshOctree::child_array_t & childOrder,
                        uint32_t nodeIndex) const;
    bool findIntersectionNode(const Ray & ray, float minDistance,
                              uint32_t & bestTriangle, float & bestDistance,
                              const TriangleMeshOctree::child_array_t & childOrder,
                              uint32_t nodeIndex) const;

    void build();

    // Add a new node and return its index
    uint32_t addNode(uint8_t level);

    void printNodes() const;
    bool nodesCoverAllTriangles() const;

    TriangleMesh & mesh;

    // Traversal helper
    static void childOrderForDirection(const Direction3 & d,
                                       child_array_t indices);
    static const char * octantString(child_index_t childIndex);
    static void printChildOrder(child_array_t & indices);

    // Bit masks for child indices. Used to determine which half
    // of the octree a child cell is in along each direction.
    enum { XBIT = 0x4, YBIT = 0x2, ZBIT = 0x1 };

    // Child indices (XYZ) from low/high
    enum { LLL=0, LLH, LHL, LHH, HLL, HLH, HHL, HHH };

    struct Node {
        Slab bounds;

        // Indices of child nodes. Index of 0 indicates no children.
        // order (XYZ): LLL,LLH,LHL,LHH,HLL,HLH,HHL,HHH
        child_array_t children = {};

        // Triangles owned by this node (indices into the mesh)
        uint32_t firstTriangle = 0;
        uint32_t numTriangles = 0;

        uint8_t level = 0;
        uint8_t numChildren = 0;
    };

    // Nodes of the octree. First is the root.
    std::vector<Node> nodes;

    // Indices into the triangle mesh. Octree nodes index into this
    // array instead of directly into the mesh because there may be
    // triangles duplicated in multiple nodes and we want to keep
    // nodes small, so we only store a range of indices into this array.
    std::vector<uint32_t> triangles;

    // Build a node by claiming the appropriate triangles from the array
    void buildNode(uint32_t nodeIndex,
                   const std::vector<uint32_t> & tris,
                   const Slab & bounds);
    void buildChild(Node & node,
                    child_index_t childIndex,
                    const std::vector<uint32_t> & childTris,
                    const Slab & childBounds);

    // Build configuration
    uint32_t buildCutOffNumTriangles = 32;
    uint8_t buildMaxLevel = 8;
};


#endif
