#include <stdlib.h>
#include <iostream>
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
#include "Renderer.h"

std::atomic<bool> flushImmediate(false); // Flush the color output as soon as possible

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
        unsigned int maxDepth = 10;
        float sensorScaleFactor = 1.0f;
        bool noMonteCarloRefraction = false;
        float russianRouletteChance = 0.1f;
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
    argParser.addArgument('p', "sensorscale", options.sensorScaleFactor);
    argParser.addArgument('r', "rr", options.russianRouletteChance);
    argParser.addFlag('R', "nomontecarlorefraction", options.noMonteCarloRefraction);

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

    printf("Samples per pixel: %d\n", options.samplesPerPixel);
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

    if(options.sensorScaleFactor != 1.0f) {
        printf("Overriding sensor size to %.2f %% of original\n", options.sensorScaleFactor * 100.0f);
        scene.sensor.pixelwidth *= options.sensorScaleFactor;
        scene.sensor.pixelheight *= options.sensorScaleFactor;
        printf("New sensor size %u x %u\n", scene.sensor.pixelwidth, scene.sensor.pixelheight);
    }

    Artifacts artifacts(scene.sensor.pixelwidth, scene.sensor.pixelheight);
    const float minDistance = 0.0f;

    RNG rng[options.numThreads];

    // Jitter offsets (applied the same to all corresponding pixel samples)
    vec2 jitter[options.samplesPerPixel];
    std::generate(jitter, jitter + options.samplesPerPixel, [&]() { return rng[0].uniformRectangle(-0.5f, 0.5f, -0.5f, 0.5f); });

    Renderer renderer;
    renderer.maxDepth = options.maxDepth;
    renderer.monteCarloRefraction = !options.noMonteCarloRefraction;
    renderer.russianRouletteChance = options.russianRouletteChance;

    auto tracePixel = [&](size_t x, size_t y, size_t threadIndex) {
        ProcessorTimer pixelTimer = ProcessorTimer::makeRunningTimer();
        const vec2 pixelCenter = vec2(x, y) + vec2(0.5f, 0.5f);

        for(unsigned int samplesIndex = 0; samplesIndex < options.samplesPerPixel; ++samplesIndex) {
            vec2 jitteredPixel = pixelCenter + jitter[samplesIndex];
            auto standardPixel = scene.sensor.pixelStandardImageLocation(jitteredPixel);

            vec2 randomBlurCoord = rng[threadIndex].uniformUnitCircle();
            auto ray = scene.camera->rayThroughStandardImagePlane(standardPixel, randomBlurCoord);

            RayIntersection intersection;
            RadianceRGB pixelRadiance;
            bool hit = renderer.traceCameraRay(scene, rng[threadIndex], ray, minDistance, 1, { VaccuumMedium }, intersection, pixelRadiance);
            artifacts.accumPixelRadiance(x, y, pixelRadiance);
            if(hit) {
                artifacts.setIntersection(x, y, minDistance, scene, intersection);
            }
        }
        artifacts.setTime(x, y, pixelTimer.elapsed());

        if(flushImmediate.exchange(false)) {
            printf("Flushing artifacts\n");
            artifacts.writeAll();
        }
    };

    printf("Monte Carlo refraction = %s\n", renderer.monteCarloRefraction ? "ON" : "OFF");
    printf("Russian Roulette chance = %.2f\n", renderer.russianRouletteChance);
    printf("Max depth = %u\n", renderer.maxDepth);
    printf("Tracing scene\n");
    auto traceTimer = WallClockTimer::makeRunningTimer();
    uint32_t tileSize = 8;
    if(options.numThreads == 1) {
        //scene.sensor.forEachPixel(tracePixel);
        scene.sensor.forEachPixelTiled(tracePixel, tileSize);
    }
    else {
        //scene.sensor.forEachPixelThreaded(tracePixel, options.numThreads);
        scene.sensor.forEachPixelTiledThreaded(tracePixel, tileSize, options.numThreads);
    }
    double traceTime = traceTimer.elapsed();
    printf("Scene traced in %f sec\n", traceTime);

    printf("Writing artifacts\n");
    auto artifactWriteTimer = WallClockTimer::makeRunningTimer();
    artifacts.writeAll();
    auto artifactWriteTime = artifactWriteTimer.elapsed();
    printf("Artifacts written in %f sec\n", artifactWriteTime);

    // TEMP
    scene.environmentMap->saveDebugSampleImage();

    return EXIT_SUCCESS;
}

