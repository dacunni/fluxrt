#include <iostream>
#include <string>
#include <limits>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define STLLOADER_IMPLEMENTATION
#include <stlloader.h>

#include "trianglemesh.h"
#include "triangle.h"
#include "ray.h"
#include "barycentric.h"
#include "coordinate.h"
#include "slab.h"
#include "filesystem.h"

// Special texcoord index indicating no texture coordinates exist for the vertex
const uint32_t NoTexCoord = std::numeric_limits<uint32_t>::max();

//
// Wavefront OBJ format
//
static void loadMaterialsFromOBJ(MaterialArray & materials,
                                 std::vector<MaterialID> & objMatToMatArrIndex,
                                 TextureCache & textureCache,
                                 std::vector<tinyobj::material_t> & objmaterials,
                                 const std::string & path)
{
    objmaterials.push_back(tinyobj::material_t()); // default material

    objMatToMatArrIndex.resize(objmaterials.size());

    for(int mi = 0; mi < objmaterials.size(); ++mi) {
        auto & objmaterial = objmaterials[mi];
        auto & D = objmaterial.diffuse;
        auto & S = objmaterial.specular;
        printf("    material %2d "
               "ior %.3f "
               "D %.1f %.1f %.1f Dt '%s' "
               "S %.1f %.1f %.1f St '%s' "
               "At '%s' dissolve %.1f"
               "\n",
               mi,
               objmaterial.ior,
               D[0], D[1], D[2], objmaterial.diffuse_texname.c_str(),
               S[0], S[1], S[2], objmaterial.specular_texname.c_str(),
               objmaterial.alpha_texname.c_str(), objmaterial.dissolve);

        auto material = Material::makeDiffuseSpecular(D, S);

        if(!objmaterial.diffuse_texname.empty()) {
            material.diffuseTexture = textureCache.loadTextureFromFile(path, objmaterial.diffuse_texname);
        }
        if(!objmaterial.specular_texname.empty()) {
            material.specularTexture = textureCache.loadTextureFromFile(path, objmaterial.specular_texname);
        }
        if(!objmaterial.alpha_texname.empty()) {
            material.alphaTexture = textureCache.loadTextureFromFile(path, objmaterial.alpha_texname);
        }

        objMatToMatArrIndex[mi] = materials.size();
        materials.push_back(material);
    }
}

static bool loadTriangleMeshFromOBJ(TriangleMesh & mesh,
                                    MaterialArray & materials,
                                    TextureCache & textureCache,
                                    const std::string & path, const std::string & filename)
{
    std::string warn, err;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> objmaterials;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &objmaterials, &warn, &err,
                                (path + '/' + filename).c_str(), path.c_str());
    if(!warn.empty()) {
        std::cout << "WARN: " << warn << std::endl;
    }
    if(!err.empty()) {
        std::cerr << err << std::endl;
    }

    if(!ret) {
        std::cerr << "Failed to load " << filename << std::endl;
        return false;
    }

    printf("OBJ Mesh:");
    printf("  vertices: %d", (int) (attrib.vertices.size()) / 3);
    printf("  normals: %d", (int) (attrib.normals.size()) / 3);
    printf("  texcoords: %d", (int) (attrib.texcoords.size()) / 2);
    printf("  materials: %d", (int) objmaterials.size());
    printf("  shapes: %d\n", (int) shapes.size());

    // Keep a mapping from original material indices to the ones we insert into
    // the materials array
    std::vector<MaterialID> objMatToMatArrIndex;

    loadMaterialsFromOBJ(materials, objMatToMatArrIndex, textureCache, objmaterials, path);

    for(int vi = 0; vi < attrib.vertices.size() / 3; ++vi) {
        auto coord = &attrib.vertices[vi * 3];
        auto pos = Position3(coord[0], coord[1], coord[2]);
        mesh.vertices.push_back(pos);
    }

    for(int ni = 0; ni < attrib.normals.size() / 3; ++ni) {
        auto coord = &attrib.normals[ni * 3];
        auto dir = Direction3(coord[0], coord[1], coord[2]);
        dir.normalize();
        if(dir.isZeros()) {
            printf("WARNING: Mesh normal at index %d is all zeros. Replacing with 0,1,0\n", ni);
            dir = Direction3(0.0f, 1.0f, 0.0f);
        }
        mesh.normals.push_back(dir);
    }

    for(int ti = 0; ti < attrib.texcoords.size() / 2; ++ti) {
        auto coord = &attrib.texcoords[ti * 2];
        auto tc = TextureCoordinate{coord[0], 1.0f - coord[1]};
        mesh.texcoords.push_back(tc);
    }

    // shapes
    for(size_t si = 0; si < shapes.size(); ++si) {
        const auto & shape = shapes[si];
        const auto num_faces = shape.mesh.indices.size() / 3;
        const auto num_materials = shape.mesh.material_ids.size();
        //printf("    shape %d faces %d materials %d\n", (int) si, (int) num_faces, (int) num_materials);

        assert(num_materials == num_faces); // per-face material

        // faces
        for(size_t fi = 0; fi < num_faces; ++fi) {
            auto indices = &shape.mesh.indices[3 * fi];
            MaterialID materialId = objMatToMatArrIndex[shape.mesh.material_ids[fi]];
            mesh.faces.material.push_back(materialId);
            // vertex indices
            for (int vi = 0; vi < 3; ++vi) {
                mesh.indices.vertex.push_back(indices[vi].vertex_index);
                mesh.indices.normal.push_back(indices[vi].normal_index);
                if(indices[vi].texcoord_index < 0) {
                    mesh.indices.texcoord.push_back(NoTexCoord);
                }
                else {
                    mesh.indices.texcoord.push_back(indices[vi].texcoord_index);
                }
            }
        }
    }

    Slab bounds = boundingBox(mesh.vertices);
    printf("Mesh bounds: "); bounds.print();

    // TODO: Handle missing normals

    return true;
}

static bool loadTriangleMeshFromSTL(TriangleMesh & mesh,
                                    MaterialArray & materials,
                                    TextureCache & textureCache,
                                    const std::string & path, const std::string & filename)
{
    stlloader::Mesh stlmesh;
    stlloader::parse_file((path + '/' + filename).c_str(), stlmesh);
    //stlloader::print(stlmesh);

    printf("STL Mesh:");
    printf("  facets: %d\n", (int) (stlmesh.facets.size()) / 3);

    // We use +Y as up in most scenes, but most STL files use +Z as up
    // for 3D printing applications. Rotate about X to remap +Z to +Y.
    auto remapOrientation = [](const vec3 & v) {
        return vec3(v.x, v.z, -v.y);
    };

    int vertex_index = 0;
    for(const auto & facet : stlmesh.facets) {
        auto & fn = facet.normal;
        auto normal = Direction3(remapOrientation(vec3(fn.x, fn.y, fn.z)));
        mesh.faces.material.push_back(NoMaterial);
        for(int vi = 0; vi < 3; ++vi) {
            auto & coord = facet.vertices[vi];
            auto pos = Position3(remapOrientation(vec3(coord.x, coord.y, coord.z)));
            mesh.vertices.push_back(pos);
            mesh.normals.push_back(normal);
            mesh.indices.vertex.push_back(vertex_index);
            mesh.indices.normal.push_back(vertex_index);
            mesh.indices.texcoord.push_back(NoTexCoord);
            ++vertex_index;
        }
    }

    Slab bounds = boundingBox(mesh.vertices);
    printf("Mesh bounds: "); bounds.print();

    return true;
}

bool loadTriangleMesh(TriangleMesh & mesh,
                      MaterialArray & materials,
                      TextureCache & textureCache,
                      const std::string & pathToFile)
{
    std::string path, filename;
    std::tie(path, filename) = filesystem::splitFileDirectory(pathToFile);

    if(filesystem::hasExtension(filename, ".obj")) {
        return loadTriangleMeshFromOBJ(mesh, materials, textureCache, path, filename);
    }
    else if(filesystem::hasExtension(filename, ".stl")) {
        return loadTriangleMeshFromSTL(mesh, materials, textureCache, path, filename);
    }

    std::cerr << "Unrecognized mesh type " << filename << '\n';
    return false;
}

bool intersects(const Ray & ray, const TriangleMesh & mesh, float minDistance)
{
    return intersectsTrianglesIndexed(ray, &mesh.vertices[0], &mesh.indices.vertex[0], mesh.indices.vertex.size(), minDistance);
}

bool findIntersection(const Ray & ray, const TriangleMesh & mesh,
                      float minDistance, RayIntersection & intersection)
{
    float bestDistance = FLT_MAX, t = FLT_MAX;
    uint32_t bestTriangle = 0;
    bool hit = false;

    auto vertex = [&mesh](uint32_t tri, uint32_t index) { return mesh.triangleVertex(tri, index); };
    const auto numTriangles = mesh.numTriangles();

    for(uint32_t tri = 0; tri < numTriangles; ++tri) {
        if(intersectsTriangle(ray, vertex(tri, 0), vertex(tri, 1), vertex(tri, 2), minDistance, &t)
           && t < bestDistance) {
            bestTriangle = tri;
            bestDistance = t;
            hit = true;
        }
    }

    if(!hit)
        return false;

    fillTriangleMeshIntersection(ray, mesh, bestTriangle, bestDistance, intersection);

    return true;
}

void fillTriangleMeshIntersection(const Ray & ray, const TriangleMesh & mesh,
                                  uint32_t tri, float t, RayIntersection & intersection)
{
    intersection.distance = t;
    intersection.position = ray.origin + ray.direction * t;

    intersection.material = mesh.faces.material[tri];

    auto bary = barycentricForPointInTriangle(intersection.position,
                                              mesh.triangleVertex(tri, 0),
                                              mesh.triangleVertex(tri, 1),
                                              mesh.triangleVertex(tri, 2));

    if(mesh.hasNormals()) {
#if 1
        // TODO: Remove this once we condition input normals to be
        //       pointing consistently in the same direction.
        Direction3 n0 = mesh.triangleNormal(tri, 0);
        Direction3 n1 = mesh.triangleNormal(tri, 1);
        Direction3 n2 = mesh.triangleNormal(tri, 2);
        if(dot(n0, ray.direction) > 0.0f) { n0.negate(); }
        if(dot(n1, ray.direction) > 0.0f) { n1.negate(); }
        if(dot(n2, ray.direction) > 0.0f) { n2.negate(); }
        intersection.normal = interpolate(n0, n1, n2, bary);
#else
        intersection.normal = interpolate(mesh.triangleNormal(tri, 0),
                                          mesh.triangleNormal(tri, 1),
                                          mesh.triangleNormal(tri, 2), bary);
#endif
    }
    else {
        // Generate normals from triangle vertices
        // TODO: This will give flat shading. It would be nice to generate
        //       per vertex normals on mesh load so we can just interpolate
        //       here as usual.
        auto v0 = mesh.triangleVertex(tri, 0);
        auto v1 = mesh.triangleVertex(tri, 1);
        auto v2 = mesh.triangleVertex(tri, 2);
        intersection.normal = cross(v2 - v0, v1 - v0).normalized();
    }

    // Interpolate texture coordinates, if available
    auto tci0 = mesh.indices.texcoord[3 * tri + 0];
    auto tci1 = mesh.indices.texcoord[3 * tri + 1];
    auto tci2 = mesh.indices.texcoord[3 * tri + 2];

    if(tci0 != NoTexCoord && tci1 != NoTexCoord && tci2 != NoTexCoord) {
        intersection.texcoord = interpolate(mesh.texcoords[tci0],
                                            mesh.texcoords[tci1],
                                            mesh.texcoords[tci2], bary);
        intersection.hasTexCoord = true;
    }
    else {
        intersection.texcoord = { 0.0f, 0.0f };
        intersection.hasTexCoord = false;
    }

    // generate tangent / bitangent
    coordinate::coordinateSystem(intersection.normal, intersection.tangent, intersection.bitangent);
}

size_t TriangleMesh::numTriangles() const
{
    return indices.vertex.size() / 3;
}

bool TriangleMesh::hasNormals() const
{
    return normals.size() > 0;
}

const Position3 & TriangleMesh::triangleVertex(uint32_t tri, uint32_t index) const
{
    return vertices[indices.vertex[3 * tri + index]];
}

const Direction3 & TriangleMesh::triangleNormal(uint32_t tri, uint32_t index) const
{
    return normals[indices.normal[3 * tri + index]];
}

const TextureCoordinate & TriangleMesh::triangleTextureCoordinate(uint32_t tri, uint32_t index) const
{
    return texcoords[indices.texcoord[3 * tri + index]];
}

void TriangleMesh::printMeta() const
{
    printf("TriangleMesh vertices %lu normals %lu indices v %lu n %lu\n",
           vertices.size(), normals.size(), indices.vertex.size(), indices.normal.size());
}

void TriangleMesh::scaleToFit(const Slab & bounds)
{
    Slab old = boundingBox(vertices);

    auto s = relativeScale(old, bounds);
    auto mine = s.minElement();
    auto maxe = s.maxElement();

    float scaleFactor = 1.0f;

    if(maxe < 1.0f) {
        // scale down
        scaleFactor = maxe;
    }
    else {
        // scale up
        scaleFactor = mine;
    }

    auto oldCenter = old.midpoint();
    auto newCenter = bounds.midpoint();

    // Scale to fit and center in the new bounding volume
    for(auto & v : vertices) {
        v.x = (v.x - oldCenter.x) * scaleFactor + newCenter.x;
        v.y = (v.y - oldCenter.y) * scaleFactor + newCenter.y;
        v.z = (v.z - oldCenter.z) * scaleFactor + newCenter.z;
    }

    // NOTE: We don't need to adjust normals be cause we are scaling uniformly in all directions
}

