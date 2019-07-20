#include <stdlib.h>
#include <iostream>
#include <vector>

#include "scene.h"

std::string sceneString = R"toml(
[camera]
type = "pinhole"
position = [ 2.3, 4.6, 1.0 ]
lookat = [ 0.2, 0.3, 0.5 ]
up = [ 0.0, 0.0, 1.0 ]
hfov = 45

[sensor]
pixelwidth = 512
pixelheight = 256

[[meshes]]
name = "monkey1"
file = "models/blender/monkey_simple_smooth.obj"

	[[meshes.transforms]]
	translate = [ -3.4, 4.7, 5.2 ]
	
	[[meshes.transforms]]
	scale = [ 4.2, 4.2, 4.2 ]
	
	[[meshes.transforms]]
	rotation.angle = 0.24
	rotation.axis = [ 1.3, 2.3, 0.2 ]

[[meshes]]
name = "sphere_mesh_1"
file = "models/blender/sphere.obj"

[[spheres]]
radius = 3.2

[[slabs]]
min = [ 3.2, 4.2, 5.1 ]
max = [ 5.6, 7.6, 9.1 ]
)toml";

int main(int argc, char ** argv)
{
    printf("Testing scene loading\n");

    std::string sceneFile = "scenes/toml/test1.toml";

    if(argc > 1) {
        sceneFile = argv[1];
    }

    Scene scene;

#if 0
    printf("Scene string: \n%s\n", sceneString.c_str());

    if(!loadSceneFromTOMLString(scene, sceneString)) {
        std::cerr << "Error loading scene\n";
        return EXIT_FAILURE;
    }

#else
    printf("Scene file: %s\n", sceneFile.c_str());

    if(!loadSceneFromFile(scene, sceneFile)) {
        std::cerr << "Error loading scene\n";
        return EXIT_FAILURE;
    }
#endif

    printf("Scene loaded successfully\n");

    return EXIT_SUCCESS;
}

