#include <stdlib.h>
#include <iostream>
#include <vector>

#include "scene.h"
#include "image.h"
#include "interpolation.h"
#include "artifacts.h"

int main(int argc, char ** argv)
{
    if(argc < 2) {
        std::cerr << "Scene argument required\n";
        return EXIT_FAILURE;
    }

    std::string sceneFile = argv[1];

    Scene scene;

    if(!loadSceneFromFile(scene, sceneFile)) {
        std::cerr << "Error loading scene\n";
        return EXIT_FAILURE;
    }

    printf("Scene loaded successfully\n");

    Artifacts artifacts(scene.sensor.pixelwidth, scene.sensor.pixelheight);
    const float minDistance = 0.01f;

    auto tracePixel = [&](size_t x, size_t y) {
        auto standardPixel = scene.sensor.pixelStandardImageLocation(float(x) + 0.5f, float(y) + 0.5f);
        auto ray = scene.camera->rayThroughStandardImagePlane(standardPixel.x, standardPixel.y);
        RayIntersection intersection;
        if(findIntersection(ray, scene, minDistance, intersection)) {
            artifacts.setIntersection(x, y, minDistance, scene, intersection);
        }
    };

    printf("Tracing scene\n");
    scene.sensor.forEachPixel(tracePixel);
    printf("Writing artifacts\n");
    artifacts.writeAll();

    return EXIT_SUCCESS;
}

