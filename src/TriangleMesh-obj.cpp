#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "TriangleMesh.h"
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
        auto & E = objmaterial.emission;
        printf("  material %2d "
               "    illum_model %d ior %.3f\n"
               "    D %.1f %.1f %.1f Dt '%s'\n"
               "    S %.1f %.1f %.1f St '%s'\n"
               "    E %.1f %.1f %.1f Et '%s'\n"
               "    At '%s' dis %.1f\n"
               "    sh %.1f\n",
               mi,
               objmaterial.illum,
               objmaterial.ior,
               D[0], D[1], D[2], objmaterial.diffuse_texname.c_str(),
               S[0], S[1], S[2], objmaterial.specular_texname.c_str(),
               E[0], E[1], E[2], objmaterial.emissive_texname.c_str(),
               objmaterial.alpha_texname.c_str(), objmaterial.dissolve,
               objmaterial.shininess
               );

        Material material;
       
        // Note: We ignore the ambient term, as it is non-physical
        switch(objmaterial.illum) {
            case 1: // Lambertian diffuse
                material = Material::makeDiffuse(D);
                break;
            case 2: // diffuse + specular (Blinn-Phong)
                material = Material::makeDiffuseSpecular(D, S);
                // TODO 
                break;
            //case 3: // diffuse + specular (Blinn-Phong) + Whitted
                //material = Material::makeDiffuseSpecular(D, S);
                // TODO
                //break;
            //case 4: // glassy ???
                //material = Material::makeDiffuseSpecular(D, S);
                // TODO
                //break;
            //case 5: // diffuse + specular (Blinn-Phong) + Fresnel
                // TODO
                //break;
            //case 6: // diffuse + specular (Blinn-Phong) + refraction (optical density)
                // TODO
                //break;
            case 7: // diffuse + specular (Blinn-Phong) + refraction (Fresnel)
                // TODO - incomplete
                // WAR: Some models have an IOR of 1, which causes no
                //      refraction and thus transparent models. We default
                //      to crown glass if this is the case.
                if(objmaterial.ior <= 1.01f) {
                    objmaterial.ior = 1.52;
                }
                material = Material::makeRefractive(objmaterial.ior);
                break;
            //case 8: // TODO
            //    break;
            //case 9: // TODO
            //    break;
            //case 10: // TODO
            //    break;
            default:
                std::cerr << "OBJ illum type " << objmaterial.illum << " not implemented\n";
                material = Material::makeDiffuseSpecular(D, S);
        }

        // FIXME: This is fine, but we should add a way to disable emission
        // in loaded models so we can test without the lights they include.
        //material.emissionColor = RadianceRGB(E);

        material.specularExponent = objmaterial.shininess;

        if(!objmaterial.diffuse_texname.empty()) {
            material.diffuseTexture = textureCache.loadTextureFromFile(path, objmaterial.diffuse_texname);
            // If the texture is RGBA, get alpha from the txture
            if(textureCache.textures[material.diffuseTexture]->numChannels == 4) {
                material.alphaTexture = material.diffuseTexture;
            }
        }
        if(!objmaterial.specular_texname.empty()) {
            material.specularTexture = textureCache.loadTextureFromFile(path, objmaterial.specular_texname);
        }
        if(!objmaterial.alpha_texname.empty()) {
            material.alphaTexture = textureCache.loadTextureFromFile(path, objmaterial.alpha_texname);
        }

        material.opacity = objmaterial.dissolve;

        objMatToMatArrIndex[mi] = materials.size();
        materials.push_back(material);
    }
}

bool loadTriangleMeshFromOBJ(TriangleMesh & mesh,
                             MaterialArray & materials,
                             TriangleMeshDataCache & meshDataCache,
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

    mesh.meshData = std::make_shared<TriangleMeshData>();
    auto & meshData = *mesh.meshData;

    for(int vi = 0; vi < attrib.vertices.size() / 3; ++vi) {
        auto coord = &attrib.vertices[vi * 3];
        auto pos = Position3(coord[0], coord[1], coord[2]);
        meshData.vertices.push_back(pos);
    }
    meshData.bounds = ::boundingBox(meshData.vertices);

    for(int ni = 0; ni < attrib.normals.size() / 3; ++ni) {
        auto coord = &attrib.normals[ni * 3];
        auto dir = Direction3(coord[0], coord[1], coord[2]);
        dir.normalize();
        if(dir.isZeros()) {
#if 0
            printf("WARNING: Mesh normal at index %d is all zeros. Replacing with 0,1,0\n", ni);
#endif
            dir = Direction3(0.0f, 1.0f, 0.0f);
        }
        meshData.normals.push_back(dir);
    }

    for(int ti = 0; ti < attrib.texcoords.size() / 2; ++ti) {
        auto coord = &attrib.texcoords[ti * 2];
        auto tc = TextureCoordinate{coord[0], 1.0f - coord[1]};
        meshData.texcoords.push_back(tc);
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
            meshData.faces.material.push_back(materialId);
            // vertex indices
            for (int vi = 0; vi < 3; ++vi) {
                // FIXME - Handle -1 for missing vertex_index, normal_index, texcoord_index
                meshData.indices.vertex.push_back(indices[vi].vertex_index);
                meshData.indices.normal.push_back(indices[vi].normal_index);
                if(indices[vi].texcoord_index < 0) {
                    meshData.indices.texcoord.push_back(TriangleMeshData::NoTexCoord);
                }
                else {
                    meshData.indices.texcoord.push_back(indices[vi].texcoord_index);
                }
            }
        }
    }

    Slab bounds = boundingBox(meshData.vertices);
    printf("Mesh bounds: "); bounds.print();

    // TODO: Handle missing normals

    return true;
}

