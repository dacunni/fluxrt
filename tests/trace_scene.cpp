#include <stdlib.h>
#include <iostream>
#include <vector>
#include <chrono>

#include "scene.h"
#include "image.h"
#include "interpolation.h"
#include "artifacts.h"
#include "constants.h"
#include "rng.h"

float computeAmbientOcclusion(Scene & scene, const RayIntersection & intersection, float minDistance, RNG & rng)
{
    const float standoff = minDistance;
    Position3 p = intersection.position + intersection.normal * standoff;
    Direction3 d;
    int numSamples = 10;
    float ao = 0.0f;
    for(int i = 0; i < numSamples; ++i) {
        rng.uniformSurfaceUnitHalfSphere(intersection.normal, d);
        Ray aoShadowRay(intersection.position + intersection.normal * 0.01, d);
        ao += intersects(aoShadowRay, scene, minDistance) ? 0.0f : dot(d, intersection.normal);
    }
    ao /= numSamples;
    //ao *= constants::PI;
    return ao;
}

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

    bool option_computeAmbientOcclusion = false;
    RNG rng;

    auto tracePixel = [&](size_t x, size_t y) {
        auto standardPixel = scene.sensor.pixelStandardImageLocation(float(x) + 0.5f, float(y) + 0.5f);
        auto ray = scene.camera->rayThroughStandardImagePlane(standardPixel.x, standardPixel.y);
        RayIntersection intersection;
        if(findIntersection(ray, scene, minDistance, intersection)) {
            artifacts.setIntersection(x, y, minDistance, scene, intersection);

            if(option_computeAmbientOcclusion) {
                float ao = computeAmbientOcclusion(scene, intersection, minDistance, rng);
                artifacts.setAmbientOcclusion(x, y, ao);
            }
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

