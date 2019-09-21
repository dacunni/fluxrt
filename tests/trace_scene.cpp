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
#include "ambientocclusion.h"
#include "timer.h"
#include "argparse.h"
#include "radiometry.h"

int main(int argc, char ** argv)
{
    CommandLineArgumentParser argParser;

    struct {
        unsigned int samplesPerPixel = 1;
        struct {
            bool compute = false;
            bool sampleCosineLobe = false;
            unsigned int numSamples = 10;
        } ambientOcclusion;
    } options;

    // General
    argParser.addArgument('s', "spp", options.samplesPerPixel);

    // Ambient Occlusion
    argParser.addFlag('a', "ao", options.ambientOcclusion.compute);
    argParser.addFlag('c', "aocosine", options.ambientOcclusion.sampleCosineLobe);
    argParser.addArgument('S', "aosamples", options.ambientOcclusion.numSamples);

    argParser.parse(argc, argv);

    auto arguments = argParser.unnamedArguments();

    if(arguments.size() < 1) {
        std::cerr << "Scene argument required\n";
        return EXIT_FAILURE;
    }

    std::string sceneFile = arguments[0];

    auto sceneLoadTimer = WallClockTimer::makeRunningTimer();
    Scene scene;
    if(!loadSceneFromFile(scene, sceneFile)) {
        std::cerr << "Error loading scene\n";
        return EXIT_FAILURE;
    }
    double sceneLoadTime = sceneLoadTimer.elapsed();

    printf("Scene loaded in %f sec\n", sceneLoadTime);

    Artifacts artifacts(scene.sensor.pixelwidth, scene.sensor.pixelheight);
    const float minDistance = 0.01f;

    RNG rng;

    // Jitter offsets (applied the same to all corresponding pixel samples)
    std::vector<vec2> jitter(options.samplesPerPixel);
    std::generate(begin(jitter), end(jitter), [&]() { return rng.uniformRectangle(-0.5f, 0.5f, -0.5f, 0.5f); });

    auto traceRay = [&](size_t x, size_t y, const Ray & ray, RayIntersection & intersection) {
        if(findIntersection(ray, scene, minDistance, intersection)) {
            artifacts.setIntersection(x, y, minDistance, scene, intersection);

            if(options.ambientOcclusion.compute) {
                float ao = computeAmbientOcclusion(scene, intersection, minDistance, rng,
                                                   options.ambientOcclusion.numSamples,
                                                   options.ambientOcclusion.sampleCosineLobe);
                artifacts.setAmbientOcclusion(x, y, ao);
            }

            // TEMP - TODO - implement real shading
            radiometry::RadianceRGB Lrgb;

            // Sample according to cosine lobe about the normal
            const float epsilon = 1.0e-4;
            Position3 p = intersection.position + intersection.normal * epsilon;
            Direction3 d(rng.cosineAboutDirection(intersection.normal));
            Ray shadowRay(p, d);
            if(intersects(shadowRay, scene, minDistance)) {
                // TODO - recurse
            }
            else {
                Lrgb = scene.environmentMap->sampleRay(shadowRay);
            }

            if(intersection.material != NoMaterial) {
                auto & material = scene.materials[intersection.material];
                auto D = material.diffuse(scene.textures, intersection.texcoord);
                return D * Lrgb;
            }
            else {
                return Lrgb;
            }
        }
        else {
            return scene.environmentMap->sampleRay(ray);
        }
    };

    auto tracePixel = [&](size_t x, size_t y) {
        ProcessorTimer pixelTimer;
        pixelTimer.start();
        vec2 pixelCenter = vec2(x, y) + vec2(0.5f, 0.5f);

        for(unsigned int samplesIndex = 0; samplesIndex < options.samplesPerPixel; ++samplesIndex) {
            vec2 jitteredPixel = pixelCenter + jitter[samplesIndex];
            auto standardPixel = scene.sensor.pixelStandardImageLocation(jitteredPixel);
            auto ray = scene.camera->rayThroughStandardImagePlane(standardPixel);
            RayIntersection intersection;
            //traceRay(x, y, ray, intersection);
            auto pixelRadiance = traceRay(x, y, ray, intersection);
            artifacts.accumPixelRadiance(x, y, pixelRadiance);
        }
        double pixelElapsed = pixelTimer.elapsed();
        artifacts.setTime(x, y, pixelElapsed);

        //if(x == 0 && (y + 1) % (scene.sensor.pixelheight / 10) == 0) {
        //    artifacts.writePixelColor();
        //}
    };

    printf("Tracing scene\n");
    auto traceTimer = WallClockTimer::makeRunningTimer();
    scene.sensor.forEachPixel(tracePixel);
    double traceTime = traceTimer.elapsed();
    printf("Scene traced in %f sec\n", traceTime);

    printf("Writing artifacts\n");
    auto artifactWriteTimer = WallClockTimer::makeRunningTimer();
    artifacts.writeAll();
    auto artifactWriteTime = artifactWriteTimer.elapsed();
    printf("Artifacts written in %f sec\n", artifactWriteTime);

    return EXIT_SUCCESS;
}

