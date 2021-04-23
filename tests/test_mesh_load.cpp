#include <stdlib.h>
#include <iostream>
#include <vector>

#include "slab.h"
#include "TriangleMesh.h"
#include "TriangleMeshOctree.h"

#include "filesystem.h"

int main(int argc, char ** argv)
{
    printf("Testing mesh loading\n");

    std::string modelFile = "models/blender/sphere.obj";

    if(argc > 1) {
        modelFile = argv[1];
    }

    auto mesh = std::make_shared<TriangleMesh>();
    MaterialArray materials;
    TextureCache textureCache;
    if(!loadTriangleMesh(*mesh, materials, textureCache, modelFile)) {
        std::cerr << "Error loading mesh\n";
        return EXIT_FAILURE;
    }
    printf("Mesh meta: "); mesh->printMeta();
    auto bounds = boundingBox(mesh->vertices);
    printf("Mesh bounds: "); bounds.print();

    printf("\nOctree Test\n");
    auto octree = std::make_shared<TriangleMeshOctree>(mesh);
    printf("Building octree\n");
    octree->build();
    //octree->printNodes();
    printf("Octree has full coverage: %s\n", octree->nodesCoverAllTriangles() ? "YES" : "NO");


    return EXIT_SUCCESS;
}

