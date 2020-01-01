#define STLLOADER_IMPLEMENTATION
#include <stlloader.h>

#include "trianglemesh.h"
#include "slab.h"

bool loadTriangleMeshFromSTL(TriangleMesh & mesh,
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
            mesh.indices.texcoord.push_back(TriangleMesh::NoTexCoord);
            ++vertex_index;
        }
    }

    Slab bounds = boundingBox(mesh.vertices);
    printf("Mesh bounds: "); bounds.print();

    return true;
}

