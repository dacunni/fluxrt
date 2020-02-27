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
#include "renderer.h"

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
        unsigned int maxDepth = 1;
        float sensorScaleFactor = 1.0f;
        bool noMonteCarloRefraction = false;
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

    std::vector<RNG> rng(options.numThreads);

    // Jitter offsets (applied the same to all corresponding pixel samples)
    std::vector<vec2> jitter(options.samplesPerPixel);
    std::generate(begin(jitter), end(jitter), [&]() { return rng[0].uniformRectangle(-0.5f, 0.5f, -0.5f, 0.5f); });

    Renderer renderer;
    renderer.maxDepth = options.maxDepth;
    renderer.monteCarloRefraction = !options.noMonteCarloRefraction;

    auto tracePixel = [&](size_t x, size_t y, size_t threadIndex) {
        ProcessorTimer pixelTimer = ProcessorTimer::makeRunningTimer();
        const vec2 pixelCenter = vec2(x, y) + vec2(0.5f, 0.5f);

        float focusDistance = 20.0f;
        float focusDivergence = focusDistance * 0.02f;

        for(unsigned int samplesIndex = 0; samplesIndex < options.samplesPerPixel; ++samplesIndex) {
            vec2 jitteredPixel = pixelCenter + jitter[samplesIndex];
            auto standardPixel = scene.sensor.pixelStandardImageLocation(jitteredPixel);

            vec2 randomBlurCoord = rng[threadIndex].uniformUnitCircle();
            auto ray = scene.camera->rayThroughStandardImagePlane(standardPixel, randomBlurCoord);

            RayIntersection intersection;
            radiometry::RadianceRGB pixelRadiance;
            bool hit = renderer.traceRay(scene, rng[threadIndex], ray, minDistance, 1, { VaccuumMedium }, intersection, pixelRadiance);
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
    printf("Max depth = %u\n", renderer.maxDepth);
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

