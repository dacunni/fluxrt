#include <stdlib.h>
#include <iostream>
#include <vector>

#include "slab.h"
#include "trianglemesh.h"
#include "trianglemeshoctree.h"

int main(int argc, char ** argv)
{
    printf("Testing mesh loading\n");

    TriangleMesh mesh;
    std::vector<Material> materials;
    //if(!loadTriangleMesh(mesh, materials, "models/blender", "sphere.obj")) {
    if(!loadTriangleMesh(mesh, materials, "models/blender", "monkey2.obj")) {
        std::cerr << "Error loading mesh\n";
        return EXIT_FAILURE;
    }
    printf("Mesh meta: "); mesh.printMeta();
    auto bounds = boundingBox(mesh.vertices);
    printf("Mesh bounds: "); bounds.print();

    printf("\nOctree Test\n");
    TriangleMeshOctree octree(mesh);
    printf("Building octree\n");
    octree.build();
    //octree.printNodes();
    printf("Octree has full coverage: %s\n", octree.nodesCoverAllTriangles() ? "YES" : "NO");


    return EXIT_SUCCESS;
}

