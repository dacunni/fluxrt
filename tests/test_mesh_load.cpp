#include <stdlib.h>
#include <iostream>
#include "micromath.h"

int main(int argc, char ** argv)
{
    printf("Testing mesh loading\n");

    TriangleMesh mesh;
    if(!loadTriangleMesh(mesh, "models/blender", "sphere.obj")) {
        std::cerr << "Error loading mesh\n";
        return EXIT_FAILURE;
    }
    printf("Mesh meta: "); mesh.printMeta();
    auto bounds = boundingBox(mesh.vertices);
    printf("Mesh bounds: "); bounds.print();

    return EXIT_SUCCESS;
}

