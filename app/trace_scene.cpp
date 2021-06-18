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
#include "Logger.h"

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
        bool verbose = false;
        unsigned int flushTimeout = 0;
        unsigned int numThreads = 1;
        unsigned int samplesPerPixel = 1;
        float epsilon = Renderer::DEFAULT_EPSILON;
        unsigned int maxDepth = Renderer::DEFAULT_MAX_DEPTH;
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
    argParser.addFlag('v', "verbose", options.verbose);
    argParser.addArgument('f', "flushtimeout", options.flushTimeout);
    argParser.addArgument('t', "threads", options.numThreads);
    argParser.addArgument('s', "spp", options.samplesPerPixel);
    argParser.addArgument('e', "epsilon", options.epsilon);
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

    auto logger = std::make_shared<FileLogger>("trace.log");
    setLogger(logger);

    if(options.verbose) {
        logger->mirrorToStdout = true;
    }

    auto arguments = argParser.unnamedArguments();

    if(arguments.size() < 1) {
        std::cerr << "Scene argument required\n";
        return EXIT_FAILURE;
    }

    printf("Flush timeout: %d sec\n", options.flushTimeout);
    printf("Samples per pixel: %d\n", options.samplesPerPixel);
    printf("Epsilon: %f\n", options.epsilon);
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

    printf("Building scene graph accelerators\n");
    scene.buildAccelerators();

    Artifacts artifacts(scene.sensor.pixelwidth, scene.sensor.pixelheight);
    const float minDistance = 0.0f;

    RNG rng[options.numThreads];

    // Jitter offsets (applied the same to all corresponding pixel samples)
    vec2 jitter[options.samplesPerPixel];
    //std::generate(jitter, jitter + options.samplesPerPixel, [&]() { return rng[0].uniformRectangle(-0.5f, 0.5f, -0.5f, 0.5f); });
    std::generate(jitter, jitter + options.samplesPerPixel, [&]() { return rng[0].gaussian2D(0.5f); });

    Renderer renderer;
    renderer.epsilon = options.epsilon;
    renderer.maxDepth = options.maxDepth;
    renderer.monteCarloRefraction = !options.noMonteCarloRefraction;
    renderer.russianRouletteChance = options.russianRouletteChance;
    renderer.shadeSpecularParams.samplePhongLobe = !options.noSampleSpecularLobe;

    auto tracePixelRay = [&](size_t x, size_t y, size_t threadIndex, uint32_t sampleIndex) {
        const vec2 pixelCenter = vec2(x, y) + vec2(0.5f, 0.5f);
        vec2 jitteredPixel = pixelCenter + jitter[sampleIndex];
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
    };

    auto renderPixelAllSamples = [&](size_t x, size_t y, size_t threadIndex) {
        ProcessorTimer pixelTimer = ProcessorTimer::makeRunningTimer();
        for(unsigned int sampleIndex = 0; sampleIndex < options.samplesPerPixel; ++sampleIndex) {
            tracePixelRay(x, y, threadIndex, sampleIndex);
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
    //scene.sensor.forEachPixelThreaded(renderPixelAllSamples, options.numThreads);
    scene.sensor.forEachPixelTiledThreaded(renderPixelAllSamples, tileSize, options.numThreads);
    double traceTime = traceTimer.elapsed();
    printf("Scene traced in %s\n", hoursMinutesSeconds(traceTime).c_str());

    artifacts.writeAll();

    // TEMP
    //scene.environmentMap->saveDebugSampleImage();

    return EXIT_SUCCESS;
}

