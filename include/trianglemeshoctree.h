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

    TriangleMesh & mesh;

    std::vector<uint32_t> triangles;

    struct Node {
        uint32_t numChildren() const;

        // Indices of child nodes. Index of 0 indicates no children.
        // order (XYZ): LLL,LLH,LHL,LHH,HLL,HLH,HHL,HHH
        uint32_t children[8];

        // Triangles owned by this node (indices into the mesh)
        uint32_t firstTriangle = 0;
        uint32_t numTriangles = 0;

        uint32_t level = 0;
    };

    // Nodes of the octree. First is the root.
    std::vector<Node> nodes;

    // Build configuration
    uint32_t buildCutOffNumTriangles = 32;
    uint32_t buildMaxLevel = 6;

    // Add a new node and return its index
    uint32_t addNode();

    void printNodes() const;
    bool nodesCoverAllTriangles() const;

    struct NodeSortRecord {
        uint32_t triangleIndex;
        uint32_t childNodeIndex;
    };

    // Build a node by claiming the appropriate triangles from the array
    void buildNode(uint32_t nodeIndex,
                   std::vector<NodeSortRecord> & records,
                   std::vector<NodeSortRecord>::iterator first,
                   std::vector<NodeSortRecord>::iterator last,
                   const Slab & bounds);
};

#endif
