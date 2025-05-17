#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "TriangleMesh.h"
#include "slab.h"
#include "Logger.h"

//
// Wavefront OBJ format
//
// References:
//   http://paulbourke.net/dataformats/obj/
//   http://paulbourke.net/dataformats/mtl/
// 
static void loadMaterialsFromOBJ(MaterialArray & materials,
                                 std::vector<MaterialID> & objMatToMatArrIndex,
                                 TextureCache & textureCache,
                                 std::vector<tinyobj::material_t> & objmaterials,
                                 const std::string & path)
{
    auto & logger = getLogger();

    objmaterials.push_back(tinyobj::material_t()); // default material

    objMatToMatArrIndex.resize(objmaterials.size());

    for(size_t mi = 0; mi < objmaterials.size(); ++mi) {
        auto & objmaterial = objmaterials[mi];
        auto & D = objmaterial.diffuse;
        auto & S = objmaterial.specular;
        auto & E = objmaterial.emission;
        logger.normalf("material %2d (%s) illum_model %d ior %.3f", mi, objmaterial.name.c_str(), objmaterial.illum, objmaterial.ior);
        logger.normalf(" D %.1f %.1f %.1f Dt '%s'", D[0], D[1], D[2], objmaterial.diffuse_texname.c_str());
        logger.normalf(" S %.1f %.1f %.1f St '%s'", S[0], S[1], S[2], objmaterial.specular_texname.c_str());
        logger.normalf(" E %.1f %.1f %.1f Et '%s'", E[0], E[1], E[2], objmaterial.emissive_texname.c_str());
        logger.normalf(" Nt '%s'", objmaterial.bump_texname.c_str());
        logger.normalf(" At '%s' dissolve %.1f shininess %.1f", objmaterial.alpha_texname.c_str(), objmaterial.dissolve, objmaterial.shininess);

        Material material;
       
        // Note: We ignore the ambient term, as it is non-physical
        switch(objmaterial.illum) {
            case 0: // Constant color - interpret as Lambertian diffuse
                material = Material::makeDiffuse(D);
                break;
            case 1: // Lambertian diffuse
                material = Material::makeDiffuse(D);
                break;
            case 2: // diffuse + specular (Blinn-Phong)
            case 3: // diffuse + specular (Blinn-Phong) + Whitted
            case 4: // glassy
                material = Material::makeDiffuseSpecular(D, S);
                break;
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
            case 8:
            case 9:
                material = Material::makeDiffuseSpecular(D, S);
                break;
            //case 10: // TODO
            //    break;
            default:
                logger.warning() << "OBJ illum type " << objmaterial.illum << " not implemented";
                material = Material::makeDiffuseSpecular(D, S);
        }

        // FIXME: This is fine, but we should add a way to disable emission
        // in loaded models so we can test without the lights they include.
        //material.emissionColor = RadianceRGB(E);

        material.specularExponentParam = objmaterial.shininess;

        if(!objmaterial.diffuse_texname.empty()) {
            material.diffuseParam = textureCache.loadTextureFromFile(path, objmaterial.diffuse_texname);
            // If the texture is RGBA, get alpha from the txture
            if(textureCache.textures[material.diffuseParam.textureId]->numChannels == 4) {
                material.alphaParam = material.diffuseParam.textureId;
            }
        }
        if(!objmaterial.specular_texname.empty()) {
            material.specularParam = textureCache.loadTextureFromFile(path, objmaterial.specular_texname);
        }
        if(!objmaterial.alpha_texname.empty()) {
            material.alphaParam = textureCache.loadTextureFromFile(path, objmaterial.alpha_texname);
        }
        if(!objmaterial.bump_texname.empty()) {
            // Despite the name, bump map seems to hold a normal map in OBJ files
            material.normalMapTexture = textureCache.loadTextureFromFile(path, objmaterial.bump_texname);
            auto & texture = textureCache.textures[material.normalMapTexture];
            // Convert normal map color to normal [0, 1] -> [-1, 1]
            texture->forEachPixelChannel([](Texture & image, size_t x, size_t y, int c) {
                                             image.set(x, y, c, image.get(x, y, c) * 2.0f - 1.0f);
                                         });
        }

        material.opacity = objmaterial.dissolve;
        vec3 transmittance(objmaterial.transmittance);
        if(!transmittance.isOnes()) {
            // FIXME: Per-channel opacity
            material.opacity = transmittance.minElement();
        }

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
    auto & logger = getLogger();

    std::string warn, err;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> objmaterials;

    logger.normal("Reading OBJ with tinyobjloader");
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &objmaterials, &warn, &err,
                                (path + '/' + filename).c_str(), path.c_str());
    if(!warn.empty()) {
        logger.warning() << warn;
    }
    if(!err.empty()) {
        logger.error() << err;
    }

    if(!ret) {
        logger.error() << "Failed to load " << filename;
        return false;
    }

    logger.normalf("OBJ Mesh: vertices: %d normals: %d texcoords: %d"
                   "  materials: %d shapes: %d",
                   (int) (attrib.vertices.size()) / 3,
                   (int) (attrib.normals.size()) / 3,
                   (int) (attrib.texcoords.size()) / 2,
                   (int) objmaterials.size(),
                   (int) shapes.size());

    // Keep a mapping from original material indices to the ones we insert into
    // the materials array
    std::vector<MaterialID> objMatToMatArrIndex;

    loadMaterialsFromOBJ(materials, objMatToMatArrIndex, textureCache, objmaterials, path);

    mesh.meshData = std::make_shared<TriangleMeshData>();
    auto & meshData = *mesh.meshData;

    // reserve memory
    meshData.vertices.reserve(attrib.vertices.size() / 3);
    meshData.normals.reserve(attrib.normals.size() / 3);
    meshData.texcoords.reserve(attrib.texcoords.size() / 2);
    size_t num_indices = 0;
    for(size_t si = 0; si < shapes.size(); ++si) {
        num_indices += shapes[si].mesh.indices.size();
    }
    meshData.indices.vertex.reserve(num_indices);
    meshData.indices.normal.reserve(num_indices);
    meshData.indices.texcoord.reserve(num_indices);

    // load vertices
    for(size_t vi = 0; vi < attrib.vertices.size() / 3; ++vi) {
        auto coord = &attrib.vertices[vi * 3];
        meshData.vertices.emplace_back(coord[0], coord[1], coord[2]);
    }
    meshData.bounds = ::boundingBox(meshData.vertices);

    // load normals
    for(size_t ni = 0; ni < attrib.normals.size() / 3; ++ni) {
        auto coord = &attrib.normals[ni * 3];
        auto dir = Direction3(coord[0], coord[1], coord[2]);
        dir.normalize();
        if(dir.isZeros()) {
#if 0
            logger.warningf("Mesh normal at index %d is all zeros. Replacing with 0,1,0", ni);
#endif
            dir = Direction3(0.0f, 1.0f, 0.0f);
        }
        meshData.normals.push_back(dir);
    }

    // load texture coordinates
    for(size_t ti = 0; ti < attrib.texcoords.size() / 2; ++ti) {
        auto coord = &attrib.texcoords[ti * 2];
        auto tc = TextureCoordinate{coord[0], 1.0f - coord[1]};
        meshData.texcoords.push_back(tc);
    }

    // shapes
    for(size_t si = 0; si < shapes.size(); ++si) {
        const auto & shape = shapes[si];
        const auto num_faces = shape.mesh.indices.size() / 3;
        const auto num_materials = shape.mesh.material_ids.size();

        assert(num_materials == num_faces); // per-face material

        // faces
        for(size_t fi = 0; fi < num_faces; ++fi) {
            auto indices = &shape.mesh.indices[3 * fi];
            size_t matIndex = shape.mesh.material_ids[fi];
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
    logger.normalf("Mesh bounds: "); bounds.log(logger);

    // TODO: Handle missing normals

    return true;
}

