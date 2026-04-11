#include <gtest/gtest.h>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <string>

#include "TriangleMesh.h"

namespace {

// Write binary content to /tmp/plytest.ply and return a loaded TriangleMesh.
static bool loadPLY(TriangleMesh & mesh, const std::string & content)
{
    {
        std::ofstream f("/tmp/plytest.ply", std::ios::binary);
        f.write(content.data(), content.size());
    }
    MaterialArray mats;
    TriangleMeshDataCache cache;
    TextureCache tc;
    return loadTriangleMeshFromPLY(mesh, mats, cache, tc, "/tmp", "plytest.ply");
}

// ---- ASCII tests ----

TEST(PLYLoading, ASCII_SingleTriangle_VertexAndTriangleCount)
{
    const std::string ply =
        "ply\n"
        "format ascii 1.0\n"
        "element vertex 3\n"
        "property float x\n"
        "property float y\n"
        "property float z\n"
        "element face 1\n"
        "property list uchar int vertex_indices\n"
        "end_header\n"
        "0 0 0\n"
        "1 0 0\n"
        "0 1 0\n"
        "3 0 1 2\n";

    TriangleMesh mesh;
    ASSERT_TRUE(loadPLY(mesh, ply));
    EXPECT_EQ(mesh.meshData->vertices.size(), 3u);
    EXPECT_EQ(mesh.numTriangles(), 1u);
}

TEST(PLYLoading, ASCII_WithNormals_NormalsLoaded)
{
    const std::string ply =
        "ply\n"
        "format ascii 1.0\n"
        "element vertex 3\n"
        "property float x\n"
        "property float y\n"
        "property float z\n"
        "property float nx\n"
        "property float ny\n"
        "property float nz\n"
        "element face 1\n"
        "property list uchar int vertex_indices\n"
        "end_header\n"
        "0 0 0  0 0 1\n"
        "1 0 0  0 0 1\n"
        "0 1 0  0 0 1\n"
        "3 0 1 2\n";

    TriangleMesh mesh;
    ASSERT_TRUE(loadPLY(mesh, ply));
    EXPECT_EQ(mesh.meshData->normals.size(), 3u);
    EXPECT_TRUE(mesh.hasNormals());
    // Each normal should be approximately (0, 0, 1)
    for(const auto & n : mesh.meshData->normals) {
        EXPECT_NEAR(n.x, 0.0f, 1e-5f);
        EXPECT_NEAR(n.y, 0.0f, 1e-5f);
        EXPECT_NEAR(n.z, 1.0f, 1e-5f);
    }
}

TEST(PLYLoading, ASCII_WithoutNormals_NormalsEmpty)
{
    const std::string ply =
        "ply\n"
        "format ascii 1.0\n"
        "element vertex 3\n"
        "property float x\n"
        "property float y\n"
        "property float z\n"
        "element face 1\n"
        "property list uchar int vertex_indices\n"
        "end_header\n"
        "0 0 0\n"
        "1 0 0\n"
        "0 1 0\n"
        "3 0 1 2\n";

    TriangleMesh mesh;
    ASSERT_TRUE(loadPLY(mesh, ply));
    EXPECT_EQ(mesh.meshData->normals.size(), 0u);
    EXPECT_FALSE(mesh.hasNormals());
}

TEST(PLYLoading, ASCII_WithTexCoords_TexCoordsLoaded)
{
    const std::string ply =
        "ply\n"
        "format ascii 1.0\n"
        "element vertex 3\n"
        "property float x\n"
        "property float y\n"
        "property float z\n"
        "property float s\n"
        "property float t\n"
        "element face 1\n"
        "property list uchar int vertex_indices\n"
        "end_header\n"
        "0 0 0  0.0 0.0\n"
        "1 0 0  1.0 0.0\n"
        "0 1 0  0.0 1.0\n"
        "3 0 1 2\n";

    TriangleMesh mesh;
    ASSERT_TRUE(loadPLY(mesh, ply));
    ASSERT_EQ(mesh.meshData->texcoords.size(), 3u);
    EXPECT_NEAR(mesh.meshData->texcoords[1].u, 1.0f, 1e-5f);
    EXPECT_NEAR(mesh.meshData->texcoords[1].v, 0.0f, 1e-5f);
    EXPECT_NEAR(mesh.meshData->texcoords[2].u, 0.0f, 1e-5f);
    EXPECT_NEAR(mesh.meshData->texcoords[2].v, 1.0f, 1e-5f);
}

TEST(PLYLoading, ASCII_QuadFace_TriangulatedIntoTwoTriangles)
{
    // A unit square as a single quad face → should become 2 triangles
    const std::string ply =
        "ply\n"
        "format ascii 1.0\n"
        "element vertex 4\n"
        "property float x\n"
        "property float y\n"
        "property float z\n"
        "element face 1\n"
        "property list uchar int vertex_indices\n"
        "end_header\n"
        "0 0 0\n"
        "1 0 0\n"
        "1 1 0\n"
        "0 1 0\n"
        "4 0 1 2 3\n";

    TriangleMesh mesh;
    ASSERT_TRUE(loadPLY(mesh, ply));
    EXPECT_EQ(mesh.meshData->vertices.size(), 4u);
    EXPECT_EQ(mesh.numTriangles(), 2u);
}

TEST(PLYLoading, ASCII_PentagonFace_TriangulatedIntoThreeTriangles)
{
    const std::string ply =
        "ply\n"
        "format ascii 1.0\n"
        "element vertex 5\n"
        "property float x\n"
        "property float y\n"
        "property float z\n"
        "element face 1\n"
        "property list uchar int vertex_indices\n"
        "end_header\n"
        "0 0 0\n"
        "1 0 0\n"
        "1 1 0\n"
        "0.5 1.5 0\n"
        "0 1 0\n"
        "5 0 1 2 3 4\n";

    TriangleMesh mesh;
    ASSERT_TRUE(loadPLY(mesh, ply));
    EXPECT_EQ(mesh.numTriangles(), 3u);
}

TEST(PLYLoading, ASCII_BoundsCorrect)
{
    const std::string ply =
        "ply\n"
        "format ascii 1.0\n"
        "element vertex 3\n"
        "property float x\n"
        "property float y\n"
        "property float z\n"
        "element face 1\n"
        "property list uchar int vertex_indices\n"
        "end_header\n"
        "-2 -3 -4\n"
        " 5  6  7\n"
        " 0  0  0\n"
        "3 0 1 2\n";

    TriangleMesh mesh;
    ASSERT_TRUE(loadPLY(mesh, ply));
    const Slab & b = mesh.meshData->bounds;
    EXPECT_FLOAT_EQ(b.xmin, -2.0f);  EXPECT_FLOAT_EQ(b.xmax, 5.0f);
    EXPECT_FLOAT_EQ(b.ymin, -3.0f);  EXPECT_FLOAT_EQ(b.ymax, 6.0f);
    EXPECT_FLOAT_EQ(b.zmin, -4.0f);  EXPECT_FLOAT_EQ(b.zmax, 7.0f);
}

TEST(PLYLoading, ASCII_UnknownVertexProperty_Skipped)
{
    // Extra "radius" property interleaved — should be ignored, xyz still parsed
    const std::string ply =
        "ply\n"
        "format ascii 1.0\n"
        "element vertex 3\n"
        "property float x\n"
        "property float y\n"
        "property float z\n"
        "property float radius\n"
        "element face 1\n"
        "property list uchar int vertex_indices\n"
        "end_header\n"
        "0 0 0  1.0\n"
        "1 0 0  2.0\n"
        "0 1 0  3.0\n"
        "3 0 1 2\n";

    TriangleMesh mesh;
    ASSERT_TRUE(loadPLY(mesh, ply));
    EXPECT_EQ(mesh.meshData->vertices.size(), 3u);
    EXPECT_EQ(mesh.numTriangles(), 1u);
}

TEST(PLYLoading, ASCII_UnknownElement_Skipped)
{
    // An "edge" element before face — should be skipped
    const std::string ply =
        "ply\n"
        "format ascii 1.0\n"
        "element vertex 3\n"
        "property float x\n"
        "property float y\n"
        "property float z\n"
        "element edge 2\n"
        "property int vertex1\n"
        "property int vertex2\n"
        "element face 1\n"
        "property list uchar int vertex_indices\n"
        "end_header\n"
        "0 0 0\n"
        "1 0 0\n"
        "0 1 0\n"
        "0 1\n"
        "1 2\n"
        "3 0 1 2\n";

    TriangleMesh mesh;
    ASSERT_TRUE(loadPLY(mesh, ply));
    EXPECT_EQ(mesh.meshData->vertices.size(), 3u);
    EXPECT_EQ(mesh.numTriangles(), 1u);
}

TEST(PLYLoading, ASCII_VertexIndexPropertyNamedVertexIndex)
{
    // Some exporters use "vertex_index" instead of "vertex_indices"
    const std::string ply =
        "ply\n"
        "format ascii 1.0\n"
        "element vertex 3\n"
        "property float x\n"
        "property float y\n"
        "property float z\n"
        "element face 1\n"
        "property list uchar int vertex_index\n"
        "end_header\n"
        "0 0 0\n"
        "1 0 0\n"
        "0 1 0\n"
        "3 0 1 2\n";

    TriangleMesh mesh;
    ASSERT_TRUE(loadPLY(mesh, ply));
    EXPECT_EQ(mesh.numTriangles(), 1u);
}

TEST(PLYLoading, BadMagic_ReturnsFalse)
{
    TriangleMesh mesh;
    EXPECT_FALSE(loadPLY(mesh, "notaplyfile\n"));
}

TEST(PLYLoading, MissingXYZ_ReturnsFalse)
{
    const std::string ply =
        "ply\n"
        "format ascii 1.0\n"
        "element vertex 1\n"
        "property float r\n"
        "property float g\n"
        "property float b\n"
        "element face 0\n"
        "property list uchar int vertex_indices\n"
        "end_header\n"
        "1.0 0.0 0.0\n";

    TriangleMesh mesh;
    EXPECT_FALSE(loadPLY(mesh, ply));
}

// ---- Binary little-endian tests ----

// Build a binary LE PLY in memory: one triangle, no normals
static std::string makeBinaryLE_Triangle()
{
    const std::string header =
        "ply\n"
        "format binary_little_endian 1.0\n"
        "element vertex 3\n"
        "property float x\n"
        "property float y\n"
        "property float z\n"
        "element face 1\n"
        "property list uchar int vertex_indices\n"
        "end_header\n";

    std::string data = header;

    auto appendFloat = [&](float v) {
        data.append(reinterpret_cast<const char *>(&v), 4);
    };
    auto appendInt32 = [&](int32_t v) {
        data.append(reinterpret_cast<const char *>(&v), 4);
    };
    auto appendUint8 = [&](uint8_t v) {
        data.push_back(char(v));
    };

    // Three vertices (assumes host is little-endian, which is true on x86)
    appendFloat(0.0f); appendFloat(0.0f); appendFloat(0.0f);
    appendFloat(2.0f); appendFloat(0.0f); appendFloat(0.0f);
    appendFloat(0.0f); appendFloat(3.0f); appendFloat(0.0f);

    // One face: count=3, indices=0,1,2
    appendUint8(3);
    appendInt32(0); appendInt32(1); appendInt32(2);

    return data;
}

TEST(PLYLoading, BinaryLE_Triangle_VertexAndTriangleCount)
{
    TriangleMesh mesh;
    ASSERT_TRUE(loadPLY(mesh, makeBinaryLE_Triangle()));
    EXPECT_EQ(mesh.meshData->vertices.size(), 3u);
    EXPECT_EQ(mesh.numTriangles(), 1u);
}

TEST(PLYLoading, BinaryLE_Triangle_VertexPositionsCorrect)
{
    TriangleMesh mesh;
    ASSERT_TRUE(loadPLY(mesh, makeBinaryLE_Triangle()));
    ASSERT_EQ(mesh.meshData->vertices.size(), 3u);
    EXPECT_FLOAT_EQ(mesh.meshData->vertices[1].x, 2.0f);
    EXPECT_FLOAT_EQ(mesh.meshData->vertices[2].y, 3.0f);
}

TEST(PLYLoading, BinaryLE_Triangle_BoundsCorrect)
{
    TriangleMesh mesh;
    ASSERT_TRUE(loadPLY(mesh, makeBinaryLE_Triangle()));
    const Slab & b = mesh.meshData->bounds;
    EXPECT_FLOAT_EQ(b.xmin, 0.0f); EXPECT_FLOAT_EQ(b.xmax, 2.0f);
    EXPECT_FLOAT_EQ(b.ymin, 0.0f); EXPECT_FLOAT_EQ(b.ymax, 3.0f);
    EXPECT_FLOAT_EQ(b.zmin, 0.0f); EXPECT_FLOAT_EQ(b.zmax, 0.0f);
}

} // namespace

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
