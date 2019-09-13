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

#include "argparse.h"

float computeAmbientOcclusion(Scene & scene, const RayIntersection & intersection, float minDistance, RNG & rng,
                              unsigned int numSamples, bool sampleCosineLobe)
{
    const float standoff = minDistance;
    Position3 p = intersection.position + intersection.normal * standoff;
    Direction3 d;
    float ao = 0.0f;
    for(int i = 0; i < numSamples; ++i) {
        if(sampleCosineLobe) {
            // Sample according to cosine lobe about the normal
            rng.cosineAboutDirection(intersection.normal, d);
            Ray aoShadowRay(intersection.position + intersection.normal * 0.02, d);
            ao += intersects(aoShadowRay, scene, minDistance) ? 0.0f : 1.0f;
        }
        else {
            // Sample hemisphere and scale by cosine of angle to normal
            rng.uniformSurfaceUnitHalfSphere(intersection.normal, d);
            Ray aoShadowRay(intersection.position + intersection.normal * 0.02, d);
            ao += intersects(aoShadowRay, scene, minDistance) ? 0.0f : 2.0f * dot(d, intersection.normal);
        }
    }
    ao /= numSamples;

    return ao;
}

int main(int argc, char ** argv)
{
    CommandLineArgumentParser argParser;

    struct {
        struct {
            bool compute = false;
            bool sampleCosineLobe = false;
            unsigned int numSamples = 10;
        } ambientOcclusion;
    } options;

    argParser.addFlag('a', "ao", options.ambientOcclusion.compute);
    argParser.addFlag('c', "aocosine", options.ambientOcclusion.sampleCosineLobe);
    argParser.addArgument('s', "aosamples", options.ambientOcclusion.numSamples);
    argParser.parse(argc, argv);

    auto arguments = argParser.unnamedArguments();

    if(arguments.size() < 1) {
        std::cerr << "Scene argument required\n";
        return EXIT_FAILURE;
    }

    std::string sceneFile = arguments[0];

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

    RNG rng;

    auto tracePixel = [&](size_t x, size_t y) {
        auto standardPixel = scene.sensor.pixelStandardImageLocation(float(x) + 0.5f, float(y) + 0.5f);
        auto ray = scene.camera->rayThroughStandardImagePlane(standardPixel.x, standardPixel.y);
        RayIntersection intersection;
        if(findIntersection(ray, scene, minDistance, intersection)) {
            artifacts.setIntersection(x, y, minDistance, scene, intersection);

            if(options.ambientOcclusion.compute) {
                float ao = computeAmbientOcclusion(scene, intersection, minDistance, rng,
                                                   options.ambientOcclusion.numSamples,
                                                   options.ambientOcclusion.sampleCosineLobe);
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
    artifacts.writeAll();
    auto artifactWriteEnd = clock::now();
    std::chrono::duration<double> artifactWriteElapsed = artifactWriteEnd - artifactWriteStart;
    printf("Artifacts written in %f sec\n", artifactWriteElapsed.count());

    return EXIT_SUCCESS;
}

