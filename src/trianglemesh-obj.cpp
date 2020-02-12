#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "trianglemesh.h"
#include "slab.h"

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
               "illum %d "
               "ior %.3f "
               "D %.1f %.1f %.1f Dt '%s' "
               "S %.1f %.1f %.1f St '%s' "
               "At '%s' dissolve %.1f"
               "\n",
               mi,
               objmaterial.illum,
               objmaterial.ior,
               D[0], D[1], D[2], objmaterial.diffuse_texname.c_str(),
               S[0], S[1], S[2], objmaterial.specular_texname.c_str(),
               objmaterial.alpha_texname.c_str(), objmaterial.dissolve);

        Material material;
       
        switch(objmaterial.illum) {
            // TODO - handle other cases
            case 7:
                // TODO - incomplete
                material = Material::makeRefractive(objmaterial.ior);
                break;
            default:
                material = Material::makeDiffuseSpecular(D, S);
        }

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
bool loadTriangleMeshesFromOBJ(std::vector<TriangleMesh*> & meshes,
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

    TriangleMesh::SharedMeshData * sharedData = new TriangleMesh::SharedMeshData();

    for(int vi = 0; vi < attrib.vertices.size() / 3; ++vi) {
        auto coord = &attrib.vertices[vi * 3];
        auto pos = Position3(coord[0], coord[1], coord[2]);
        sharedData->vertices.push_back(pos);
    }

    for(int ni = 0; ni < attrib.normals.size() / 3; ++ni) {
        auto coord = &attrib.normals[ni * 3];
        auto dir = Direction3(coord[0], coord[1], coord[2]);
        dir.normalize();
        if(dir.isZeros()) {
            printf("WARNING: Mesh normal at index %d is all zeros. Replacing with 0,1,0\n", ni);
            dir = Direction3(0.0f, 1.0f, 0.0f);
        }
        sharedData->normals.push_back(dir);
    }

    for(int ti = 0; ti < attrib.texcoords.size() / 2; ++ti) {
        auto coord = &attrib.texcoords[ti * 2];
        auto tc = TextureCoordinate{coord[0], 1.0f - coord[1]};
        sharedData->texcoords.push_back(tc);
    }

    // shapes
    for(size_t si = 0; si < shapes.size(); ++si) {
        const auto & shape = shapes[si];
        const auto num_faces = shape.mesh.indices.size() / 3;
        const auto num_materials = shape.mesh.material_ids.size();
        //printf("    shape %d faces %d materials %d\n", (int) si, (int) num_faces, (int) num_materials);

        assert(num_materials == num_faces); // per-face material

        TriangleMesh * mesh = new TriangleMesh();
        mesh->sharedData = sharedData;

        // faces
        for(size_t fi = 0; fi < num_faces; ++fi) {
            auto indices = &shape.mesh.indices[3 * fi];
            auto matIndex = shape.mesh.material_ids[fi];
            MaterialID materialId = 0;
            if(matIndex >= 0 && matIndex < objMatToMatArrIndex.size()) {
                materialId = objMatToMatArrIndex[matIndex];
            }
            mesh->faces.material.push_back(materialId);
            // vertex indices
            for (int vi = 0; vi < 3; ++vi) {
                // FIXME - Handle -1 for missing vertex_index, normal_index, texcoord_index
                mesh->indices.vertex.push_back(indices[vi].vertex_index);
                mesh->indices.normal.push_back(indices[vi].normal_index);
                if(indices[vi].texcoord_index < 0) {
                    mesh->indices.texcoord.push_back(TriangleMesh::NoTexCoord);
                }
                else {
                    mesh->indices.texcoord.push_back(indices[vi].texcoord_index);
                }
            }
        }

        mesh->printMeta(); // TEMP

        meshes.push_back(mesh);
    }

    Slab bounds = boundingBox(sharedData->vertices);
    printf("Mesh bounds: "); bounds.print();

    return true;
}

#if 0
bool loadTriangleMeshFromOBJ(TriangleMesh & mesh,
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
            auto matIndex = shape.mesh.material_ids[fi];
            MaterialID materialId = 0;
            if(matIndex >= 0 && matIndex < objMatToMatArrIndex.size()) {
                materialId = objMatToMatArrIndex[matIndex];
            }
            mesh.faces.material.push_back(materialId);
            // vertex indices
            for (int vi = 0; vi < 3; ++vi) {
                // FIXME - Handle -1 for missing vertex_index, normal_index, texcoord_index
                mesh.indices.vertex.push_back(indices[vi].vertex_index);
                mesh.indices.normal.push_back(indices[vi].normal_index);
                if(indices[vi].texcoord_index < 0) {
                    mesh.indices.texcoord.push_back(TriangleMesh::NoTexCoord);
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
#endif

