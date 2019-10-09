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
#include "optics.h"
#include "fresnel.h"

bool traceRay(const Scene & scene, RNG & rng, const Ray & ray, float minDistance, unsigned int depth, unsigned int maxDepth,
              RayIntersection & intersection, radiometry::RadianceRGB & Lo)
{
    if(!findIntersection(ray, scene, minDistance, intersection)) {
        Lo = scene.environmentMap->sampleRay(ray);
        return false;
    }

    const Material & material = materialFromID(intersection.material, scene.materials);
    auto D = material.diffuse(scene.textures, intersection.texcoord);
    auto S = material.specular(scene.textures, intersection.texcoord);
    bool hasSpecular = material.hasSpecular();

    const float epsilon = 1.0e-4;
    Position3 p = intersection.position + intersection.normal * epsilon;
    const Direction3 Wi = -ray.direction;

    radiometry::RadianceRGB Ld, Ls;

    // Trace diffuse bounce
    {
        // Sample according to cosine lobe about the normal
        Direction3 d(rng.cosineAboutDirection(intersection.normal));
        Ray shadowRay(p, d);
        if(intersects(shadowRay, scene, minDistance)) {
            if(depth < maxDepth) {
                RayIntersection nextIntersection;
                radiometry::RadianceRGB Li;
                bool hitNext = traceRay(scene, rng, shadowRay, minDistance, depth + 1, maxDepth, nextIntersection, Li);
                Ld = Li;
            }
        }
        else {
            Ld = scene.environmentMap->sampleRay(shadowRay);
        }
    }

    // Trace specular bounce
    if(hasSpecular) {
        Direction3 d = mirror(Wi, intersection.normal);
        Ray shadowRay(p, d);

        if(intersects(shadowRay, scene, minDistance)) {
            if(depth < maxDepth) {
                RayIntersection nextIntersection;
                radiometry::RadianceRGB Li;
                bool hitNext = traceRay(scene, rng, shadowRay, minDistance, depth + 1, maxDepth, nextIntersection, Li);
                Ls = Li;
            }
        }
        else {
            Ls = scene.environmentMap->sampleRay(shadowRay);
        }
    }

    if(hasSpecular) {
        // Fresnel = specular
        ReflectanceRGB F0rgb = S;
        float cosIncidentAngle = absDot(Wi, intersection.normal);
        ReflectanceRGB Frgb = fresnel::schlick(F0rgb, cosIncidentAngle);
        ReflectanceRGB OmFrgb{1.0f-Frgb.r, 1.0f-Frgb.g, 1.0f-Frgb.b};
        Lo = OmFrgb * (D * Ld) + Frgb * Ls;
    }
    else {
        Lo = D * Ld;
    }

    return true;
}

int main(int argc, char ** argv)
{
    CommandLineArgumentParser argParser;

    struct {
        unsigned int samplesPerPixel = 1;
        unsigned int maxDepth = 1;
        struct {
            bool compute = false;
            bool sampleCosineLobe = false;
            unsigned int numSamples = 10;
        } ambientOcclusion;
    } options;

    // General
    argParser.addArgument('s', "spp", options.samplesPerPixel);
    argParser.addArgument('d', "maxdepth", options.maxDepth);

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

    auto tracePixel = [&](size_t x, size_t y) {
        ProcessorTimer pixelTimer = ProcessorTimer::makeRunningTimer();
        vec2 pixelCenter = vec2(x, y) + vec2(0.5f, 0.5f);

        for(unsigned int samplesIndex = 0; samplesIndex < options.samplesPerPixel; ++samplesIndex) {
            vec2 jitteredPixel = pixelCenter + jitter[samplesIndex];
            auto standardPixel = scene.sensor.pixelStandardImageLocation(jitteredPixel);
            auto ray = scene.camera->rayThroughStandardImagePlane(standardPixel);
            RayIntersection intersection;
            radiometry::RadianceRGB pixelRadiance;
            bool hit = traceRay(scene, rng, ray, minDistance, 1, options.maxDepth, intersection, pixelRadiance);
            artifacts.accumPixelRadiance(x, y, pixelRadiance);
            if(hit) {
                artifacts.setIntersection(x, y, minDistance, scene, intersection);
            }
        }
        artifacts.setTime(x, y, pixelTimer.elapsed());

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

