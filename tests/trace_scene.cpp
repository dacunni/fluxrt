#include <stdlib.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <atomic>
#include <signal.h>

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

std::atomic<bool> flushImmediate(false); // Flush the color output as soon as possible

class Renderer
{
    using RadianceRGB = radiometry::RadianceRGB;

    public:
        bool traceRay(const Scene & scene, RNG & rng, const Ray & ray, const float minDistance, const unsigned int depth,
                      RayIntersection & intersection, RadianceRGB & Lo) const;

        radiometry::RadianceRGB shade(const Scene & scene, RNG & rng, const float minDistance, const unsigned int depth,
                                      const Direction3 & Wi, RayIntersection & intersection, const Material & material) const;

    const float epsilon = 1.0e-4;
    unsigned int maxDepth = 2;
};

bool Renderer::traceRay(const Scene & scene, RNG & rng, const Ray & ray, const float minDistance, const unsigned int depth,
                        RayIntersection & intersection, RadianceRGB & Lo) const
{
    if(depth > maxDepth) {
        return false;
    }

    if(!findIntersection(ray, scene, minDistance, intersection)) {
        Lo = scene.environmentMap->sampleRay(ray);
        return false;
    }

    const Direction3 Wi = -ray.direction;

    const Material & material = materialFromID(intersection.material, scene.materials);
    auto A = material.alpha(scene.textureCache.textures, intersection.texcoord);

    if(A < 1.0f) { // transparent
        // Trace a new ray just past the intersection
        float newMinDistance = intersection.distance + epsilon;
        return traceRay(scene, rng, ray, newMinDistance, depth, intersection, Lo);
    }

    Lo = shade(scene, rng, minDistance, depth, Wi, intersection, material);

    return true;
}

radiometry::RadianceRGB Renderer::shade(const Scene & scene, RNG & rng, const float minDistance, const unsigned int depth,
                                        const Direction3 & Wi, RayIntersection & intersection, const Material & material) const
{
    if(dot(Wi, intersection.normal) < 0.0f) {
        intersection.normal.negate();
    }

    const auto D = material.diffuse(scene.textureCache.textures, intersection.texcoord);
    const auto S = material.specular(scene.textureCache.textures, intersection.texcoord);
    const bool hasSpecular = material.hasSpecular();

    const Position3 p = intersection.position + intersection.normal * epsilon;

    RadianceRGB Ld, Ls;

    // Trace diffuse bounce
    {
        // Sample according to cosine lobe about the normal
        const Direction3 d(rng.cosineAboutDirection(intersection.normal));
        const Ray nextRay(p, d);
        RayIntersection nextIntersection;
        traceRay(scene, rng, nextRay, epsilon, depth + 1, nextIntersection, Ld);
    }

    // Trace specular bounce
    if(hasSpecular) {
        const Direction3 d = mirror(Wi, intersection.normal);
        const Ray nextRay(p, d);
        RayIntersection nextIntersection;
        traceRay(scene, rng, nextRay, epsilon, depth + 1, nextIntersection, Ls);
    }

    RadianceRGB Lo;

    if(hasSpecular) {
        // Fresnel = specular
        ReflectanceRGB F0rgb = S;
        float cosIncidentAngle = absDot(Wi, intersection.normal);
        ReflectanceRGB Frgb = fresnel::schlick(F0rgb, cosIncidentAngle);
        ReflectanceRGB OmFrgb = Frgb.residual();
        Lo = OmFrgb * (D * Ld) + Frgb * Ls;
    }
    else {
        Lo = D * Ld;
    }

    return Lo;
}

void signalHandler(int signum)
{
    if(signum == SIGUSR1    // User-defined signal (kill -USR1)
#if defined(SIGINFO)
       || signum == SIGINFO // Ctrl-T
#endif
      ) {
        flushImmediate = true;
    }
    else if(signum == SIGFPE) {
        printf("WARNING: Floating point exception!\n");
    }
}

int main(int argc, char ** argv)
{
    signal(SIGUSR1, signalHandler);
#if defined(SIGINFO)
    signal(SIGINFO, signalHandler);
#endif

    CommandLineArgumentParser argParser;

    struct {
        unsigned int numThreads = 1;
        unsigned int samplesPerPixel = 1;
        unsigned int maxDepth = 1;
        struct {
            bool compute = false;
            bool sampleCosineLobe = false;
            unsigned int numSamples = 10;
        } ambientOcclusion;
    } options;

    // General
    argParser.addArgument('t', "threads", options.numThreads);
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

    printf("Number of threads: %d\n", options.numThreads);

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
    const float minDistance = 0.0f;

    std::vector<RNG> rng(options.numThreads);

    // Jitter offsets (applied the same to all corresponding pixel samples)
    std::vector<vec2> jitter(options.samplesPerPixel);
    std::generate(begin(jitter), end(jitter), [&]() { return rng[0].uniformRectangle(-0.5f, 0.5f, -0.5f, 0.5f); });

    Renderer renderer;
    renderer.maxDepth = options.maxDepth;

    auto tracePixel = [&](size_t x, size_t y, size_t threadIndex) {
        ProcessorTimer pixelTimer = ProcessorTimer::makeRunningTimer();
        const vec2 pixelCenter = vec2(x, y) + vec2(0.5f, 0.5f);

        for(unsigned int samplesIndex = 0; samplesIndex < options.samplesPerPixel; ++samplesIndex) {
            vec2 jitteredPixel = pixelCenter + jitter[samplesIndex];
            auto standardPixel = scene.sensor.pixelStandardImageLocation(jitteredPixel);
            auto ray = scene.camera->rayThroughStandardImagePlane(standardPixel);
            RayIntersection intersection;
            radiometry::RadianceRGB pixelRadiance;
            bool hit = renderer.traceRay(scene, rng[threadIndex], ray, minDistance, 1, intersection, pixelRadiance);
            artifacts.accumPixelRadiance(x, y, pixelRadiance);
            if(hit) {
                artifacts.setIntersection(x, y, minDistance, scene, intersection);
            }
        }
        artifacts.setTime(x, y, pixelTimer.elapsed());

        if(flushImmediate) {
            artifacts.writeAll();
            flushImmediate = false;
        }
    };

    printf("Tracing scene\n");
    auto traceTimer = WallClockTimer::makeRunningTimer();
    if(options.numThreads == 1) {
        scene.sensor.forEachPixel(tracePixel);
    }
    else {
        scene.sensor.forEachPixelThreaded(tracePixel, options.numThreads);
    }
    double traceTime = traceTimer.elapsed();
    printf("Scene traced in %f sec\n", traceTime);

    printf("Writing artifacts\n");
    auto artifactWriteTimer = WallClockTimer::makeRunningTimer();
    artifacts.writeAll();
    auto artifactWriteTime = artifactWriteTimer.elapsed();
    printf("Artifacts written in %f sec\n", artifactWriteTime);

    return EXIT_SUCCESS;
}

