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
    const float epsilon = 1.0e-4;
    Position3 p = intersection.position + intersection.normal * epsilon;
    Direction3 d;
    float ao = 0.0f;
    for(int i = 0; i < numSamples; ++i) {
        if(sampleCosineLobe) {
            // Sample according to cosine lobe about the normal
            rng.cosineAboutDirection(intersection.normal, d);
            Ray aoShadowRay(p, d);
            ao += intersects(aoShadowRay, scene, minDistance) ? 0.0f : 1.0f;
        }
        else {
            // Sample hemisphere and scale by cosine of angle to normal
            rng.uniformSurfaceUnitHalfSphere(intersection.normal, d);
            Ray aoShadowRay(p, d);
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

    // Jitter offsets (applied the same to all corresponding pixel samples)
    std::vector<float> jitterX(options.samplesPerPixel);
    std::vector<float> jitterY(options.samplesPerPixel);
    std::generate(begin(jitterX), end(jitterX), [&]() { return rng.uniformRange(-0.5f, 0.5f); });
    std::generate(begin(jitterY), end(jitterY), [&]() { return rng.uniformRange(-0.5f, 0.5f); });

    auto tracePixel = [&](size_t x, size_t y) {
        auto pixelStart = clock::now();
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
                float ao = computeAmbientOcclusion(scene, intersection, minDistance, rng,
                                                   options.ambientOcclusion.numSamples,
                                                   options.ambientOcclusion.sampleCosineLobe);
                color::ColorRGB pixelColor;

                if(intersection.material != NoMaterial) {
                    auto & material = scene.materials[intersection.material];
                    auto D = material.diffuse(scene.textures, intersection.texcoord);
                    pixelColor.r = D.r * ao;
                    pixelColor.g = D.g * ao;
                    pixelColor.b = D.b * ao;
                }
                else {
                    pixelColor = color::ColorRGB(ao, ao, ao);
                }

                artifacts.accumPixelColor(x, y, pixelColor);
            }
            else {
                artifacts.accumPixelColor(x, y, color::ColorRGB::BLACK());
            }
        }
        auto pixelEnd = clock::now();
        std::chrono::duration<double> pixelElapsed = pixelEnd - pixelStart;
        artifacts.setTime(x, y, pixelElapsed.count());
        //std::cout << pixelElapsed.count() << '\n';

        //if(x == 0 && (y + 1) % (scene.sensor.pixelheight / 10) == 0) {
        //    artifacts.writePixelColor();
        //}
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

