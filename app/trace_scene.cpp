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
#include "AmbientOcclusion.h"
#include "timer.h"
#include "argparse.h"
#include "Renderer.h"
#include "RenderProjection.h"
#include "Logger.h"
#include "LatLonEnvironmentMap.h"

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
        float epsilon = Renderer::DEFAULT_EPSILON_ADDITIVE;
        unsigned int maxDepth = Renderer::DEFAULT_MAX_DEPTH;
        float sensorScaleFactor = 1.0f;
        bool noMonteCarloRefraction = false;
        float russianRouletteChance = 0.1f;
        bool noSampleCosineLobe = false;
        bool noSampleSpecularLobe = false;
        std::string renderOrder = "default";
        std::string renderProjection = "";  // empty = use scene setting
        struct {
            bool compute = false;
            bool sampleCosineLobe = false;
            unsigned int numSamples = 10;
        } ambientOcclusion;
        struct {
            std::string latLonOverride;
            float scaleFactor = 1.0f;
        } envmap;
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
    argParser.addArgument('o', "renderorder", options.renderOrder);
    argParser.addArgument('T', "projection", options.renderProjection);

    // Sampling
    argParser.addFlag('C', "nosamplecosine", options.noSampleCosineLobe);
    argParser.addFlag('X', "nosamplespecular", options.noSampleSpecularLobe);

    // Ambient Occlusion
    argParser.addFlag('a', "ao", options.ambientOcclusion.compute);
    argParser.addFlag('c', "aocosine", options.ambientOcclusion.sampleCosineLobe);
    argParser.addArgument('S', "aosamples", options.ambientOcclusion.numSamples);

    // Environment Map
    argParser.addArgument('E', "envmap", options.envmap.latLonOverride);
    argParser.addArgument('F', "envmapscale", options.envmap.scaleFactor);

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

    logger->logArgv(Logger::Normal, "Command Line", argc, argv);

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

    if(!options.envmap.latLonOverride.empty()) {
        std::string file = EnvironmentMap::lookupPath(options.envmap.latLonOverride);
        logger->normal() << "Environment map override: " << file;
        logger->normal() << "Environment map scale factor " << options.envmap.scaleFactor;
        auto envmap = std::make_unique<LatLonEnvironmentMap>();
        envmap->loadFromFile(file);
        envmap->setScaleFactor(options.envmap.scaleFactor);
        scene.environmentMap = std::move(envmap);
    }

    printf("Building scene graph accelerators\n");
    scene.buildAccelerators();

    Renderer renderer;
    renderer.epsilon = options.epsilon;
    renderer.maxDepth = options.maxDepth;
    renderer.monteCarloRefraction = !options.noMonteCarloRefraction;
    renderer.russianRouletteChance = options.russianRouletteChance;
    renderer.shadeDiffuseParams.sampleCosineLobe = !options.noSampleCosineLobe;
    renderer.shadeSpecularParams.samplePhongLobe = !options.noSampleSpecularLobe;

    renderer.logConfiguration(*logger);
    renderer.printConfiguration();

    // Resolve render projection: CLI flag overrides TOML setting
    const std::string projectionName = options.renderProjection.empty()
                                       ? scene.renderProjection
                                       : options.renderProjection;
    printf("Render projection: %s\n", projectionName.c_str());

    auto renderProjection = RenderProjection::create(projectionName);

    if(options.renderOrder == "default") {
        options.renderOrder = "tiled";
    }

    auto resetFlushTimer = [&]() {
        if(options.flushTimeout != 0) {
            alarm(options.flushTimeout);
        }
    };
    resetFlushTimer();

    RenderParams renderParams;
    renderParams.numThreads      = options.numThreads;
    renderParams.samplesPerPixel = options.samplesPerPixel;
    renderParams.renderOrder     = options.renderOrder;
    renderParams.flushImmediate  = &flushImmediate;

    printf("====[ Tracing Scene ]====\n");
    auto traceTimer = WallClockTimer::makeRunningTimer();

    renderProjection->render(scene, renderer, renderParams);

    double traceTime = traceTimer.elapsed();
    printf("Scene traced in %s\n", hoursMinutesSeconds(traceTime).c_str());

    return EXIT_SUCCESS;
}
