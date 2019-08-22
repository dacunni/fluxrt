#include <stdlib.h>
#include <iostream>
#include <vector>
#include <chrono>

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

    using clock = std::chrono::system_clock;

    auto sceneLoadStart = clock::now();
    Scene scene;
    if(!loadSceneFromFile(scene, sceneFile)) {
        std::cerr << "Error loading scene\n";
        return EXIT_FAILURE;
    }
    auto sceneLoadEnd = clock::now();
    std::chrono::duration<double> sceneLoadElapsed = sceneLoadEnd - sceneLoadStart;

    printf("Scene loaded in %f sec\n", sceneLoadElapsed.count());

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
    auto traceStart = clock::now();
    scene.sensor.forEachPixel(tracePixel);
    auto traceEnd = clock::now();
    std::chrono::duration<double> traceElapsed = traceEnd - traceStart;
    printf("Scene traced in %f sec\n", traceElapsed.count());

    printf("Writing artifacts\n");
    auto artifactWriteStart = clock::now();
    scene.sensor.forEachPixel(tracePixel);
    artifacts.writeAll();
    auto artifactWriteEnd = clock::now();
    std::chrono::duration<double> artifactWriteElapsed = artifactWriteEnd - artifactWriteStart;
    printf("Artifacts written in %f sec\n", artifactWriteElapsed.count());

    return EXIT_SUCCESS;
}

