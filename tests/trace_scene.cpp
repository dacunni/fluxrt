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

    using wallclock = std::chrono::system_clock;

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
    std::vector<float> jitterX(options.samplesPerPixel);
    std::vector<float> jitterY(options.samplesPerPixel);
    std::generate(begin(jitterX), end(jitterX), [&]() { return rng.uniformRange(-0.5f, 0.5f); });
    std::generate(begin(jitterY), end(jitterY), [&]() { return rng.uniformRange(-0.5f, 0.5f); });

    auto tracePixel = [&](size_t x, size_t y) {
        ProcessorTimer pixelTimer;
        pixelTimer.start();
        float pixelCenterX = float(x) + 0.5f;
        float pixelCenterY = float(y) + 0.5f;

        for(unsigned int samplesIndex = 0; samplesIndex < options.samplesPerPixel; ++samplesIndex) {
            float jitteredPixelX = pixelCenterX + jitterX[samplesIndex];
            float jitteredPixelY = pixelCenterY + jitterY[samplesIndex];
            //float jitteredPixelX = pixelCenterX + rng.uniformRange(-0.5f, 0.5f);
            //float jitteredPixelY = pixelCenterY + rng.uniformRange(-0.5f, 0.5f);

            auto standardPixel = scene.sensor.pixelStandardImageLocation(jitteredPixelX, jitteredPixelY);

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

                // TEMP - TODO - implement real shading
                float LiR = 0.0f;
                float LiG = 0.0f;
                float LiB = 0.0f;
#if 1
                {
                    // Sample according to cosine lobe about the normal
                    const float epsilon = 1.0e-4;
                    Position3 p = intersection.position + intersection.normal * epsilon;
                    Direction3 d;
                    rng.cosineAboutDirection(intersection.normal, d);
                    Ray shadowRay(p, d);
                    if(intersects(shadowRay, scene, minDistance)) {
                        // TODO - recurse
                    }
                    else {
                        color::ColorRGB envmapColor = scene.environmentMap->sampleRay(shadowRay);
                        LiR = envmapColor.r;
                        LiG = envmapColor.g;
                        LiB = envmapColor.b;
                    }
                }
#else
                float ao = computeAmbientOcclusion(scene, intersection, minDistance, rng,
                                                   options.ambientOcclusion.numSamples,
                                                   options.ambientOcclusion.sampleCosineLobe);
                LiR = LiG = LiB = ao;
#endif

                color::ColorRGB pixelColor;

                if(intersection.material != NoMaterial) {
                    auto & material = scene.materials[intersection.material];
                    auto D = material.diffuse(scene.textures, intersection.texcoord);
                    pixelColor.r = D.r * LiR;
                    pixelColor.g = D.g * LiG;
                    pixelColor.b = D.b * LiB;
                }
                else {
                    pixelColor = color::ColorRGB(LiR, LiG, LiB);
                }

                artifacts.accumPixelColor(x, y, pixelColor);
            }
            else {
                //artifacts.accumPixelColor(x, y, color::ColorRGB::BLACK());
                artifacts.accumPixelColor(x, y, scene.environmentMap->sampleRay(ray));
            }
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

