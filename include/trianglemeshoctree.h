#ifndef __TRIANGLE_MESH_OCTREE_H__
#define __TRIANGLE_MESH_OCTREE_H__

#include <vector>
#include <cstdint>

struct TriangleMesh;
struct Slab;

struct TriangleMeshOctree
{
    TriangleMeshOctree(TriangleMesh & mesh);
    ~TriangleMeshOctree() = default;

    void build();

    // Add a new node and return its index
    uint32_t addNode(uint8_t level);

    void printNodes() const;
    bool nodesCoverAllTriangles() const;

    TriangleMesh & mesh;

    struct Node {
        uint32_t numChildren() const;

        // Indices of child nodes. Index of 0 indicates no children.
        // order (XYZ): LLL,LLH,LHL,LHH,HLL,HLH,HHL,HHH
        uint32_t children[8];

        // Triangles owned by this node (indices into the mesh)
        uint32_t firstTriangle = 0;
        uint32_t numTriangles = 0;

        uint8_t level = 0;
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
                    unsigned int childIndex,
                    const std::vector<uint32_t> & childTris,
                    const Slab & childBounds);

    // Build configuration
    uint32_t buildCutOffNumTriangles = 32;
    uint8_t buildMaxLevel = 8;
};

#endif
