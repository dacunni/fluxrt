#include <gtest/gtest.h>
#include <memory>
#include <cmath>
#include "TriangleMesh.h"
#include "TriangleMeshOctree.h"
#include "Ray.h"
#include "vectortypes.h"
#include "Triangle.h"

namespace {

// Build a simple grid mesh of NxN quads (each quad = 2 triangles)
// spanning [-halfSize, halfSize] in X and Y, at z=0.
std::shared_ptr<TriangleMesh> makeFlatGrid(int N, float halfSize)
{
    auto mesh = std::make_shared<TriangleMesh>();
    auto & data = *mesh->meshData;

    // Generate vertices: (N+1) x (N+1) grid
    for(int j = 0; j <= N; ++j) {
        for(int i = 0; i <= N; ++i) {
            float x = -halfSize + 2.0f * halfSize * i / N;
            float y = -halfSize + 2.0f * halfSize * j / N;
            data.vertices.push_back(Position3{x, y, 0.0f});
            data.normals.push_back(Direction3{0.0f, 0.0f, 1.0f});
        }
    }

    // Generate two triangles per quad
    auto idx = [&](int i, int j) -> uint32_t { return j * (N + 1) + i; };
    for(int j = 0; j < N; ++j) {
        for(int i = 0; i < N; ++i) {
            // Lower-left triangle
            data.indices.vertex.push_back(idx(i,   j  ));
            data.indices.vertex.push_back(idx(i+1, j  ));
            data.indices.vertex.push_back(idx(i+1, j+1));
            // Upper-right triangle
            data.indices.vertex.push_back(idx(i,   j  ));
            data.indices.vertex.push_back(idx(i+1, j+1));
            data.indices.vertex.push_back(idx(i,   j+1));
            // Normals, texcoords, and face materials (required by fillTriangleMeshIntersection)
            for(int k = 0; k < 6; ++k) {
                data.indices.normal.push_back(0); // all point up
                data.indices.texcoord.push_back(TriangleMeshData::NoTexCoord);
            }
            // Two triangles per quad
            data.faces.material.push_back(NoMaterial);
            data.faces.material.push_back(NoMaterial);
        }
    }

    return mesh;
}

// Brute-force intersection: check all triangles, return closest hit distance (-1 = miss)
float bruteForceIntersect(const TriangleMesh & mesh, const Ray & ray, float minDist)
{
    float bestDist = std::numeric_limits<float>::max();
    bool hit = false;
    uint32_t n = mesh.numTriangles();
    for(uint32_t ti = 0; ti < n; ++ti) {
        float t;
        auto v0 = mesh.triangleVertex(ti, 0);
        auto v1 = mesh.triangleVertex(ti, 1);
        auto v2 = mesh.triangleVertex(ti, 2);
        if(intersectsTriangle(ray, v0, v1, v2, minDist, std::numeric_limits<float>::max(), &t)) {
            if(t < bestDist) {
                bestDist = t;
                hit = true;
            }
        }
    }
    return hit ? bestDist : -1.0f;
}

// Octree intersection: return closest hit distance (-1 = miss)
float octreeIntersect(TriangleMeshOctree & octree, const Ray & ray, float minDist)
{
    RayIntersection isect;
    if(octree.findIntersection(ray, minDist, isect))
        return isect.distance;
    return -1.0f;
}

// Compare octree vs brute-force for many rays against the mesh
void compareOctreeVsBruteForce(std::shared_ptr<TriangleMesh> mesh,
                                uint32_t cutoff,
                                const std::string & label)
{
    TriangleMeshOctree octree(mesh);
    octree.buildCutOffNumTriangles = cutoff;
    octree.build();

    ASSERT_TRUE(octree.nodesCoverAllTriangles()) << label << ": not all triangles covered";

    const float minDist = 1e-4f;
    const int M = 40; // grid of rays
    const float halfSize = 1.9f; // slightly inside the mesh
    int mismatches = 0;

    for(int j = 0; j < M; ++j) {
        for(int i = 0; i < M; ++i) {
            float x = -halfSize + 2.0f * halfSize * i / (M - 1);
            float y = -halfSize + 2.0f * halfSize * j / (M - 1);

            // Ray shooting straight down at the grid (which is at z=0)
            Ray ray;
            ray.origin    = Position3{x, y, 10.0f};
            ray.direction = Direction3{0.0f, 0.0f, -1.0f};

            float bf  = bruteForceIntersect(*mesh, ray, minDist);
            float oct = octreeIntersect(octree, ray, minDist);

            bool bfHit  = bf  > 0.0f;
            bool octHit = oct > 0.0f;

            if(bfHit != octHit) {
                ++mismatches;
                ADD_FAILURE() << label << ": hit mismatch at (" << x << ", " << y << ")"
                              << " brute=" << (bfHit ? "HIT" : "MISS")
                              << " octree=" << (octHit ? "HIT" : "MISS");
            } else if(bfHit && octHit && std::fabs(bf - oct) > 1e-3f) {
                ++mismatches;
                ADD_FAILURE() << label << ": distance mismatch at (" << x << ", " << y << ")"
                              << " brute=" << bf << " octree=" << oct;
            }
        }
    }

    if(mismatches == 0) {
        SUCCEED();
    }
}

// --- Tests ---

TEST(RayTriangleMeshOctreeIntersect, SmallGridDefaultCutoff) {
    auto mesh = makeFlatGrid(4, 2.0f);
    compareOctreeVsBruteForce(mesh, 32, "4x4grid cutoff=32");
}

TEST(RayTriangleMeshOctreeIntersect, SmallGridForceSplits) {
    auto mesh = makeFlatGrid(4, 2.0f);
    compareOctreeVsBruteForce(mesh, 2, "4x4grid cutoff=2");
}

TEST(RayTriangleMeshOctreeIntersect, LargerGridForceSplits) {
    auto mesh = makeFlatGrid(8, 2.0f);
    compareOctreeVsBruteForce(mesh, 2, "8x8grid cutoff=2");
}

TEST(RayTriangleMeshOctreeIntersect, DiagonalRays) {
    auto mesh = makeFlatGrid(4, 2.0f);
    TriangleMeshOctree octree(mesh);
    octree.buildCutOffNumTriangles = 2;
    octree.build();

    const float minDist = 1e-4f;
    const int M = 20;
    const float halfSize = 1.8f;
    int mismatches = 0;

    for(int j = 0; j < M; ++j) {
        for(int i = 0; i < M; ++i) {
            float x = -halfSize + 2.0f * halfSize * i / (M - 1);
            float y = -halfSize + 2.0f * halfSize * j / (M - 1);

            // Diagonal ray from (x, y, 10) toward (0, 0, 0)
            Direction3 d = Direction3{-x, -y, -10.0f};
            float len = std::sqrt(d.x*d.x + d.y*d.y + d.z*d.z);
            d.x /= len; d.y /= len; d.z /= len;

            Ray ray;
            ray.origin    = Position3{x, y, 10.0f};
            ray.direction = d;

            float bf  = bruteForceIntersect(*mesh, ray, minDist);
            float oct = octreeIntersect(octree, ray, minDist);

            bool bfHit  = bf  > 0.0f;
            bool octHit = oct > 0.0f;

            if(bfHit != octHit) {
                ++mismatches;
                ADD_FAILURE() << "DiagRays hit mismatch at (" << x << ", " << y << ")"
                              << " brute=" << (bfHit ? "HIT" : "MISS")
                              << " octree=" << (octHit ? "HIT" : "MISS");
            } else if(bfHit && octHit && std::fabs(bf - oct) > 1e-3f) {
                ++mismatches;
                ADD_FAILURE() << "DiagRays distance mismatch at (" << x << ", " << y << ")"
                              << " brute=" << bf << " octree=" << oct;
            }
        }
    }
    if(mismatches == 0) SUCCEED();
}

// Test where triangles span across octree boundaries at various levels
TEST(RayTriangleMeshOctreeIntersect, BoundarySpanningTriangles) {
    // Create a mesh with a single large triangle straddling the center
    auto mesh = std::make_shared<TriangleMesh>();
    auto & data = *mesh->meshData;

    // Large triangle that spans all 8 octants
    data.vertices.push_back(Position3{ 2.0f,  0.0f, 0.0f});  // 0
    data.vertices.push_back(Position3{-1.0f,  1.7f, 0.0f});  // 1
    data.vertices.push_back(Position3{-1.0f, -1.7f, 0.0f});  // 2
    data.normals.push_back(Direction3{0.0f, 0.0f, 1.0f});

    data.indices.vertex = {0, 1, 2};
    data.indices.normal = {0, 0, 0};
    data.indices.texcoord = {TriangleMeshData::NoTexCoord, TriangleMeshData::NoTexCoord, TriangleMeshData::NoTexCoord};
    data.faces.material = {NoMaterial};

    TriangleMeshOctree octree(mesh);
    octree.buildCutOffNumTriangles = 1; // Force deepest split
    octree.build();

    const float minDist = 1e-4f;
    const int M = 30;
    int mismatches = 0;

    // Shoot rays at a grid covering the triangle
    for(int j = 0; j < M; ++j) {
        for(int i = 0; i < M; ++i) {
            float x = -1.5f + 3.5f * i / (M - 1);
            float y = -1.8f + 3.6f * j / (M - 1);

            Ray ray;
            ray.origin    = Position3{x, y, 5.0f};
            ray.direction = Direction3{0.0f, 0.0f, -1.0f};

            float bf  = bruteForceIntersect(*mesh, ray, minDist);
            float oct = octreeIntersect(octree, ray, minDist);

            bool bfHit  = bf  > 0.0f;
            bool octHit = oct > 0.0f;

            if(bfHit != octHit) {
                ++mismatches;
                ADD_FAILURE() << "BoundarySpan hit mismatch at (" << x << ", " << y << ")"
                              << " brute=" << (bfHit ? "HIT" : "MISS")
                              << " octree=" << (octHit ? "HIT" : "MISS");
            }
        }
    }
    if(mismatches == 0) SUCCEED();
}

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
