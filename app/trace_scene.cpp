#include <stdlib.h>
#include <unistd.h>
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
       || signum == SIGALRM // Timeout
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
    signal(SIGALRM, signalHandler);

    CommandLineArgumentParser argParser;

    struct {
        bool help = false;
        unsigned int flushTimeout = 0;
        unsigned int numThreads = 1;
        unsigned int samplesPerPixel = 1;
        unsigned int maxDepth = 10;
        float sensorScaleFactor = 1.0f;
        bool noMonteCarloRefraction = false;
        float russianRouletteChance = 0.1f;
        bool noSampleSpecularLobe = false;
        struct {
            bool compute = false;
            bool sampleCosineLobe = false;
            unsigned int numSamples = 10;
        } ambientOcclusion;
    } options;

    // General
    argParser.addFlag('h', "help", options.help);
    argParser.addArgument('f', "flushtimeout", options.flushTimeout);
    argParser.addArgument('t', "threads", options.numThreads);
    argParser.addArgument('s', "spp", options.samplesPerPixel);
    argParser.addArgument('d', "maxdepth", options.maxDepth);
    argParser.addArgument('p', "sensorscale", options.sensorScaleFactor);
    argParser.addArgument('r', "rr", options.russianRouletteChance);
    argParser.addFlag('R', "nomontecarlorefraction", options.noMonteCarloRefraction);

    // Sampling
    argParser.addFlag('X', "nosamplespecular", options.noSampleSpecularLobe);

    // Ambient Occlusion
    argParser.addFlag('a', "ao", options.ambientOcclusion.compute);
    argParser.addFlag('c', "aocosine", options.ambientOcclusion.sampleCosineLobe);
    argParser.addArgument('S', "aosamples", options.ambientOcclusion.numSamples);

    argParser.parse(argc, argv);

    if(options.help) {
        argParser.printUsage();
        return EXIT_SUCCESS;
    }

    auto arguments = argParser.unnamedArguments();

    if(arguments.size() < 1) {
        std::cerr << "Scene argument required\n";
        return EXIT_FAILURE;
    }

    printf("Flush timeout: %d sec\n", options.flushTimeout);
    printf("Samples per pixel: %d\n", options.samplesPerPixel);
    printf("Number of threads: %d\n", options.numThreads);

    printf("====[ Loading Scene ]====\n");
    std::string sceneFile = arguments[0];
    auto sceneLoadTimer = WallClockTimer::makeRunningTimer();
    Scene scene;
    if(!loadSceneFromFile(scene, sceneFile)) {
        std::cerr << "Error loading scene\n";
        return EXIT_FAILURE;
    }
    double sceneLoadTime = sceneLoadTimer.elapsed();

    printf("Scene loaded in %s\n", hoursMinutesSeconds(sceneLoadTime).c_str());

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
    //std::generate(jitter, jitter + options.samplesPerPixel, [&]() { return rng[0].uniformRectangle(-0.5f, 0.5f, -0.5f, 0.5f); });
    std::generate(jitter, jitter + options.samplesPerPixel, [&]() { return rng[0].gaussian2D(0.5f); });

    Renderer renderer;
    renderer.maxDepth = options.maxDepth;
    renderer.monteCarloRefraction = !options.noMonteCarloRefraction;
    renderer.russianRouletteChance = options.russianRouletteChance;
    renderer.shadeSpecularParams.samplePhongLobe = !options.noSampleSpecularLobe;

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
            artifacts.writeAll();
            if(options.flushTimeout != 0) {
                alarm(options.flushTimeout);
            }
        }
    };

    renderer.printConfiguration();
    printf("====[ Tracing Scene ]====\n");

    if(options.flushTimeout != 0) {
        alarm(options.flushTimeout);
    }

    auto traceTimer = WallClockTimer::makeRunningTimer();
    uint32_t tileSize = 8;
    //scene.sensor.forEachPixelThreaded(tracePixel, options.numThreads);
    scene.sensor.forEachPixelTiledThreaded(tracePixel, tileSize, options.numThreads);
    double traceTime = traceTimer.elapsed();
    printf("Scene traced in %s\n", hoursMinutesSeconds(traceTime).c_str());

    artifacts.writeAll();

    // TEMP
    //scene.environmentMap->saveDebugSampleImage();

    return EXIT_SUCCESS;
}

