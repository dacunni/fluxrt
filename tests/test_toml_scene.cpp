#include <stdlib.h>
#include <iostream>
#include <vector>

#include "scene.h"

int main(int argc, char ** argv)
{
    printf("Testing TOML scene loading\n");

    std::string sceneFile = "scenes/toml/test1.toml";

    if(argc > 1) {
        sceneFile = argv[1];
    }

    Scene scene;

    printf("Scene file: %s\n", sceneFile.c_str());

    if(!loadSceneFromTOML(scene, sceneFile)) {
        std::cerr << "Error loading scene\n";
        return EXIT_FAILURE;
    }

    printf("Scene loaded successfully\n");

    return EXIT_SUCCESS;
}

