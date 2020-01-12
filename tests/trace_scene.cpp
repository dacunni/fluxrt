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

void printDepthPrefix(unsigned int num)
{
    for(unsigned int i = 0; i < num; ++i) {
        printf("  ");
    }
}

class Renderer
{
    using RadianceRGB = radiometry::RadianceRGB;
    using IndexOfRefractionStack = std::vector<float>;

    public:
        bool traceRay(const Scene & scene, RNG & rng, const Ray & ray, const float minDistance, const unsigned int depth,
                      const IndexOfRefractionStack & iorStack,
                      RayIntersection & intersection, RadianceRGB & Lo) const;

        radiometry::RadianceRGB shade(const Scene & scene, RNG & rng, const float minDistance, const unsigned int depth,
                                      const IndexOfRefractionStack & iorStack,
                                      const Direction3 & Wi, RayIntersection & intersection, const Material & material) const;

        const float epsilon = 1.0e-4;
        unsigned int maxDepth = 2;
};

bool Renderer::traceRay(const Scene & scene, RNG & rng, const Ray & ray, const float minDistance, const unsigned int depth,
                        const IndexOfRefractionStack & iorStack,
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
        return traceRay(scene, rng, ray, newMinDistance, depth, iorStack, intersection, Lo);
    }

    Lo = shade(scene, rng, minDistance, depth, iorStack, Wi, intersection, material);

    return true;
}

radiometry::RadianceRGB Renderer::shade(const Scene & scene, RNG & rng, const float minDistance, const unsigned int depth,
                                        const IndexOfRefractionStack & iorStack,
                                        const Direction3 & Wi, RayIntersection & intersection, const Material & material) const
{
    // Notational convenience
    const auto P = intersection.position;
    auto N = intersection.normal;

    if(dot(Wi, N) < 0.0f) {
        N.negate();
    }

    const auto D = material.diffuse(scene.textureCache.textures, intersection.texcoord);
    const auto S = material.specular(scene.textureCache.textures, intersection.texcoord);
    const bool hasSpecular = material.hasSpecular();
    const bool isRefractive = material.isRefractive;
    const float nMaterial = material.indexOfRefraction;

    RadianceRGB Ld, Ls, Lt;

    if(isRefractive) {
        // Refraction
        {
            float n1, n2;

            bool leaving = iorStack.size() % 2 == 0;

            // Update IOR stack for refracted ray
            IndexOfRefractionStack nextIorStack = iorStack;
            if(leaving) {
                n1 = nMaterial;
                nextIorStack.pop_back();
                n2 = nextIorStack.back();
            }
            else {
                n1 = nextIorStack.back();
                n2 = nMaterial;
                nextIorStack.push_back(nMaterial);
            }

            Direction3 d = refract(Wi, N, n1, n2);

            bool totalInternalReflection = d.isZeros();

            if(totalInternalReflection) {
                // Reflected ray
                RayIntersection nextIntersection;
                traceRay(scene, rng, Ray(P + N * epsilon, mirror(Wi, N)),
                         epsilon, depth + 1, iorStack, nextIntersection, Ls);
            }
            else {
                float F = fresnel::dialectric::unpolarized(dot(Wi, N), dot(d, -N), n1, n2);
#if 1
                // Randomly choose a reflected or refracted ray using Fresnel as the
                // weighting factor
                if(rng.uniform01() < F) {
                    // Reflected ray
                    RayIntersection reflectIntersection;
                    traceRay(scene, rng, Ray(P + N * epsilon, mirror(Wi, N)),
                             epsilon, depth + 1, iorStack, reflectIntersection, Ls);
                }
                else {
                    // Refracted ray
                    RayIntersection refractIntersection;
                    traceRay(scene, rng, Ray(P - N * epsilon, d),
                             epsilon, depth + 1, nextIorStack, refractIntersection, Lt);
                }
#else
                // Refracted ray
                RayIntersection refractIntersection;
                traceRay(scene, rng, Ray(P - N * epsilon, d),
                         epsilon, depth + 1, nextIorStack, refractIntersection, Lt);

                // Reflected ray
                RayIntersection reflectIntersection;
                traceRay(scene, rng, Ray(P + N * epsilon, mirror(Wi, N)),
                         epsilon, depth + 1, iorStack, reflectIntersection, Ls);

                // Apply Fresnel
                Ls = F * Ls;
                Lt = (1.0f - F) * Lt;
#endif
            }
        }
    }
    else {
        // Trace diffuse bounce
        RayIntersection nextIntersection;
        traceRay(scene, rng,
                 // Sample according to cosine lobe about the normal
                 Ray(P + N * epsilon, Direction3(rng.cosineAboutDirection(N))),
                 epsilon, depth + 1, iorStack, nextIntersection, Ld);

        // Trace specular bounce
        if(hasSpecular) {
            RayIntersection nextIntersection;
            traceRay(scene, rng,
                     Ray(P + N * epsilon, mirror(Wi, N)),
                     epsilon, depth + 1, iorStack, nextIntersection, Ls);
        }
    }

    RadianceRGB Lo;

    if(isRefractive) {
        Lo = Lt + Ls;
    }
    else if(hasSpecular) {
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
        float sensorScaleFactor = 1.0f;
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

    auto tracePixel = [&](size_t x, size_t y, size_t threadIndex) {
        ProcessorTimer pixelTimer = ProcessorTimer::makeRunningTimer();
        const vec2 pixelCenter = vec2(x, y) + vec2(0.5f, 0.5f);

        for(unsigned int samplesIndex = 0; samplesIndex < options.samplesPerPixel; ++samplesIndex) {
            vec2 jitteredPixel = pixelCenter + jitter[samplesIndex];
            auto standardPixel = scene.sensor.pixelStandardImageLocation(jitteredPixel);
            auto ray = scene.camera->rayThroughStandardImagePlane(standardPixel);
            RayIntersection intersection;
            radiometry::RadianceRGB pixelRadiance;
            bool hit = renderer.traceRay(scene, rng[threadIndex], ray, minDistance, 1, {1.0f}, intersection, pixelRadiance);
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

