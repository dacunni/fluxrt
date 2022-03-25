#include "material.h"
#include "Renderer.h"
#include "Logger.h"
#include "Ray.h"
#include "rng.h"
#include "scene.h"
#include "coordinate.h"
#include "brdf.h"

void printDepthPrefix(unsigned int num)
{
    for(unsigned int i = 0; i < num; ++i) {
        printf("  ");
    }
}

bool Renderer::traceRay(const Scene & scene, RNG & rng, const Ray & ray,
                        const float minDistance, const unsigned int depth,
                        const MediumStack & mediumStack,
                        bool accumEmission,
                        bool accumEnvMap,
                        RayIntersection & intersection,
                        RadianceRGB & Lo) const
{
    if(depth > maxDepth) {
        return false;
    }

    // Check for RR termination
    if(depth >= russianRouletteMinDepth
       && rng.uniform01() < russianRouletteChance) {
        return false;
    }

    // Russian roulette factor applied if we didn't terminate early
    float RR = (depth >= russianRouletteMinDepth) ?  1.0f - russianRouletteChance : 1.0f;

    bool hit = findIntersectionWorldRay(ray, scene, minDistance, intersection);

    if(!hit) {
        if(accumEnvMap) {
            Lo = scene.environmentMap->sampleRay(ray);
        }
        Lo /= RR;
        return false;
    }

    const Direction3 Wo = -ray.direction;

    const Material & material = materialFromID(intersection.material, scene.materials);
    auto A = material.alpha(scene.textureCache.textures, intersection.texcoord);

    material.applyNormalMap(scene.textureCache.textures, intersection.texcoord,
                            intersection.normal, intersection.tangent, intersection.bitangent);

    // Transparency
    if(A < 1.0f && rng.uniform01() > A) {
        // Trace a new ray just past the intersection
        float newMinDistance = intersection.distance + epsilon;
        bool hit = traceRay(scene, rng, ray, newMinDistance, depth, mediumStack, accumEmission, accumEnvMap, intersection, Lo);
        Lo /= RR;
        return hit;
    }

    Lo = shade(scene, rng, minDistance, depth, mediumStack, Wo, intersection, material);

    // Apply Beer's Law attenuation
    ParameterRGB att = mediumStack.back().beersLawAttenuation;
    ParameterRGB beer = optics::beersLawAttenuation(att, intersection.distance);
    Lo = Lo * beer;

    // Emission
    if(accumEmission) {
        const auto E = material.emission(scene.textureCache.textures, intersection.texcoord);
        Lo += E;
    }

    // Account for RR loss
    Lo /= RR;

    return true;
}

RadianceRGB Renderer::traceRay(const Scene & scene, RNG & rng,
                               const Ray & ray,
                               const float minDistance, const unsigned int depth,
                               const MediumStack & mediumStack,
                               bool accumEmission, bool accumEnvMap) const
{
    RayIntersection intersection;
    RadianceRGB Lo;
    
    bool hit = traceRay(scene, rng, ray, minDistance, depth, mediumStack, accumEmission, accumEnvMap, intersection, Lo);

    return Lo;
}

inline RadianceRGB Renderer::shade(const Scene & scene, RNG & rng,
                                   const float minDistance,
                                   const unsigned int depth,
                                   const MediumStack & mediumStack,
                                   const Direction3 & Wo,
                                   RayIntersection & intersection,
                                   const Material & material) const
{
    // Notational convenience
    const auto P = intersection.position;
    auto N = intersection.normal;

    if(dot(Wo, N) < 0.0f) {
        N.negate();
    }

    const auto D = material.diffuse(scene.textureCache.textures, intersection.texcoord);
    const auto S = material.specular(scene.textureCache.textures, intersection.texcoord);
    const Medium & medium = material.innerMedium;

    //
    // --------------------------
    //                | specular
    // --------------------------
    //  refractive    | diffuse
    // --------------------------
    //  transmissive  |
    // --------------------------

    RadianceRGB Lo;

    // TODO
    //  - Material
    //    - distinctive types (diffuse, specular, refractive, PBR, etc)
    //    - returns MaterialInterface with BRDF
    //    - call shadeBRDF
    //    - remove custom shade* functions
    //    - add more BRDFs and materials
    //    - RGB BRDF?

    if(material.isRefractive) {
        Lo = shadeRefractiveInterface(scene, rng, minDistance, depth, mediumStack, medium, Wo, P, N);
    }
    else {
        RadianceRGB Ld, Ls;
        ReflectanceRGB F = { 0.0f, 0.0f, 0.0f };

        // Randomly choose between specular and diffuse
        // TODO: Determine the best probability
        float probSpec = material.hasSpecular() ? ((S.r + S.g + S.b) / 3.0f) : 0.0f;
        float probDiffuse = 1.0f - probSpec;
        bool doSpec = rng.uniform01() < probSpec;
        bool doDiffuse = !doSpec && material.hasDiffuse();

        // Trace specular bounce
        if(material.hasSpecular()) {
            // Fresnel = specular - TODO: Is this right?
            ReflectanceRGB F0 = S;
            F = fresnel::schlick(F0, absDot(Wo, N));
        }

        // Trace specular bounce
        if(doSpec) {
            if(material.isGlossy(scene.textureCache.textures, intersection.texcoord)) {
                float specularExponent = material.specularExponent(scene.textureCache.textures, intersection.texcoord);
                Ls = shadeSpecularGlossy(scene, rng, minDistance, depth, mediumStack, Wo, P, N, specularExponent);
            }
            else {
                Ls = shadeReflect(scene, rng, minDistance, depth, mediumStack, Wo, P, N);
            }
            Ls /= probSpec;
        }

        // Trace diffuse bounce
        if(doDiffuse) {
            Ld = shadeDiffuse(scene, rng, minDistance, depth, mediumStack, Wo, P, N);
            Ld /= probDiffuse;
        }

        Lo = F.residual() * (D * Ld) + F * Ls;
    }

    return Lo;
}

bool Renderer::traceCameraRay(const Scene & scene, RNG & rng, const Ray & ray,
                              const float minDistance, const unsigned int depth,
                              const MediumStack & mediumStack,
                              RayIntersection & intersection, RadianceRGB & Lo) const
{
    bool hit = traceRay(scene, rng, ray, minDistance, depth, mediumStack, true, true, intersection, Lo);

    if(verbose.radiance && hit) {
        printf("traceCameraRay: hit %s, Lo (%.1f, %.1f, %.1f)\n",
               hit ? "YES" : "NO", Lo.r, Lo.g, Lo.b);
    }

    return hit;
}

inline RadianceRGB Renderer::shadeReflect(const Scene & scene, RNG & rng,
                                          const float minDistance, const unsigned int depth,
                                          const MediumStack & mediumStack,
                                          const Direction3 & Wo,
                                          const Position3 & P, const Direction3 & N) const
{
    MirrorBRDF brdf;

    return shadeBRDF(scene, rng, minDistance, depth, mediumStack, Wo, P, N, brdf,
                     false,
                     0);
}


inline RadianceRGB Renderer::shadeRefract(const Scene & scene, RNG & rng,
                                          const float minDistance, const unsigned int depth,
                                          const MediumStack & mediumStack,
                                          const Direction3 & Dt,
                                          const Position3 & P, const Direction3 & N) const
{
    const Ray ray(P - N * epsilon, Dt);
    return traceRay(scene, rng, ray, epsilon, depth + 1, mediumStack, true, true);
}

inline RadianceRGB Renderer::shadeRefractiveInterface(const Scene & scene, RNG & rng,
                                                      const float minDistance, const unsigned int depth,
                                                      const MediumStack & mediumStack,
                                                      const Medium & medium,
                                                      const Direction3 & Wo,
                                                      const Position3 & P, const Direction3 & N) const
{
    RadianceRGB Ls, Lt;

    float n1, n2;

    bool leaving = mediumStack.size() % 2 == 0;

    // Update medium stack for refracted ray
    MediumStack nextMediumStack = mediumStack;
    if(leaving) {
        n1 = medium.indexOfRefraction;
        nextMediumStack.pop_back();
        n2 = nextMediumStack.back().indexOfRefraction;
    }
    else {
        n1 = nextMediumStack.back().indexOfRefraction;
        n2 = medium.indexOfRefraction;
        nextMediumStack.push_back(medium);
    }

    Direction3 d = refract(Wo, N, n1, n2);

    bool totalInternalReflection = d.isZeros();

    if(totalInternalReflection) {
        // Reflected ray
        Ls = shadeReflect(scene, rng, minDistance, depth, mediumStack, Wo, P, N);
    }
    else {
        float F = fresnel::dialectric::unpolarized(dot(Wo, N), dot(d, -N), n1, n2);

        if(monteCarloRefraction) {
            // Randomly choose a reflected or refracted ray using Fresnel as the
            // weighting factor
            if(F == 1.0f || rng.uniform01() < F) {
                Ls = shadeReflect(scene, rng, minDistance, depth, mediumStack, Wo, P, N);
            }
            else {
                Lt = shadeRefract(scene, rng, minDistance, depth, nextMediumStack, d, P, N);
            }
        }
        else {
            Ls = shadeReflect(scene, rng, minDistance, depth, mediumStack, Wo, P, N);
            Lt = shadeRefract(scene, rng, minDistance, depth, nextMediumStack, d, P, N);
            // Apply Fresnel
            Ls = F * Ls;
            Lt = (1.0f - F) * Lt;
        }
    }

    return Ls + Lt;
}

inline RadianceRGB Renderer::shadeDiffuse(const Scene & scene, RNG & rng,
                                          const float minDistance, const unsigned int depth,
                                          const MediumStack & mediumStack,
                                          const Direction3 & Wo,
                                          const Position3 & P, const Direction3 & N) const
{
    LambertianBRDF brdf;
    brdf.importanceSample = shadeDiffuseParams.sampleCosineLobe;

    return shadeBRDF(scene, rng, minDistance, depth, mediumStack, Wo, P, N, brdf,
                     shadeDiffuseParams.sampleLights,
                     shadeDiffuseParams.numEnvMapSamples);
}

inline RadianceRGB Renderer::shadeSpecularGlossy(const Scene & scene, RNG & rng,
                                                 const float minDistance, const unsigned int depth,
                                                 const MediumStack & mediumStack,
                                                 const Direction3 & Wo,
                                                 const Position3 & P, const Direction3 & N,
                                                 float exponent) const
{
    PhongBRDF brdf(exponent);
    brdf.importanceSample = shadeSpecularParams.samplePhongLobe;

    return shadeBRDF(scene, rng, minDistance, depth, mediumStack, Wo, P, N, brdf,
                     shadeSpecularParams.sampleLights,
                     shadeSpecularParams.numEnvMapSamples);
}

inline RadianceRGB Renderer::shadeBRDF(const Scene & scene, RNG & rng,
                                       const float minDistance, const unsigned int depth,
                                       const MediumStack & mediumStack,
                                       const Direction3 & Wo,
                                       const Position3 & P, const Direction3 & N,
                                       BRDF & brdf,
                                       bool sampleLights,
                                       unsigned int numEnvMapSamples) const
{
    RadianceRGB Lo;

    // Note: We don't accumulate emission on the next hit if we sample
    //       lighting directly, to avoid double counting direct illumination.

    const bool sampleEnvMap =
        numEnvMapSamples > 0
        && scene.environmentMap->canImportanceSample();

    if(sampleLights) {
        Lo += sampleAllPointLights(scene, rng, brdf, Wo, P, N, epsilon);
        Lo += sampleAllDiskLights(scene, rng, brdf, Wo, P, N, epsilon);
    }

    if(sampleEnvMap) {
        Lo += sampleEnvironmentMap(scene, rng, brdf, Wo, P, N, minDistance, numEnvMapSamples);
    }

    brdfSample S = brdf.sample(rng.uniform2DRange01(), Wo, N);

    RadianceRGB Li = traceRay(scene, rng, Ray(P + N * epsilon, S.W), epsilon, depth + 1, mediumStack,
                              !sampleLights, !sampleEnvMap);
    float F = brdf.eval(Wo, S.W, N);
    float D = S.isDelta() ? 1.0f : clampedDot(S.W, N);

    Lo += Li * F * D / S.pdf;

    if(verbose.radiance) {
        printf("shadeBRDF: Lo (%.1f, %.1f, %.1f) = Li (%.1f, %.1f, %.1f) * F (%.1f) * D (%.1f) / pdf (%.1f)\n",
               Lo.r, Lo.g, Lo.b, Li.r, Li.g, Li.b, F, D, S.pdf);
    }

    return Lo;

}

inline LightSample Renderer::samplePointLight(const Scene & scene,
                                              RNG & rng,
                                              const PointLight & light,
                                              const Position3 & P,
                                              const Direction3 & N,
                                              float minDistance) const
{
    const Direction3 toLight = light.position - P;
    LightSample S;
    S.L = RadianceRGB::BLACK();
    S.direction = toLight.normalized();

    const float lightDistSq = toLight.magnitude_sq();
    const float lightDist = std::sqrt(lightDistSq);

    // Make sure the light is on the right size of the surface
    if(dot(toLight, N) < 0.0f) {
        return S;
    }

    // If we hit something, we can't see the light
    if(intersectsScene(scene, rng, Ray{P, S.direction}, epsilon, lightDist - epsilon)) {
        return S;
    }

    S.L = light.intensity / lightDistSq;

    return S;
}


inline RadianceRGB Renderer::sampleAllPointLights(const Scene & scene,
                                                  RNG & rng,
                                                  const BRDF & brdf,
                                                  const Direction3 & Wo,
                                                  const Position3 & P,
                                                  const Direction3 & N,
                                                  float minDistance) const
{
    RadianceRGB Lo;

    // Sample point lights
    for(const auto & light: scene.pointLights) {
        LightSample S = samplePointLight(scene, rng, light, P, N, epsilon);
        float F = brdf.eval(Wo, S.direction, N);
        Lo += F * S.L * clampedDot(S.direction, N);
    }

    return Lo;
}

inline LightSample Renderer::sampleDiskLight(const Scene & scene,
                                             RNG & rng,
                                             const DiskLight & light,
                                             const Position3 & P,
                                             const Direction3 & N,
                                             float minDistance) const
{
    vec2 offset = rng.uniformCircle(light.radius);
    // rotate to align with direction
    vec3 ax1, ax2;
    coordinate::coordinateSystem(light.direction, ax1, ax2);
    vec3 pointOnDisk = offset.x * ax1 + offset.y * ax2;
    Position3 pointOnLight = light.position + Direction3(pointOnDisk);
    Direction3 toLight = pointOnLight - P;

    LightSample S;
    S.L = RadianceRGB::BLACK();
    S.direction = toLight.normalized();

    // Make sure the light is on the right size of the surface
    if(dot(toLight, N) < 0.0f) {
        return S;
    }

    const float lightDistSq = toLight.magnitude_sq();
    const float lightDist = std::sqrt(lightDistSq);

    // If we hit something, we can't see the light
    if(intersectsScene(scene, rng, Ray{P, S.direction}, epsilon, lightDist - epsilon)) {
        return S;
    }

    const Material & material = materialFromID(light.material, scene.materials);
    RayIntersection lightIntersection; // FIXME - dummy for texcoords that we don't support yet
    auto E = material.emission(scene.textureCache.textures, lightIntersection.texcoord);

    // Scale by the solid angle of the light as seen by the shaded point
    float A = constants::PI * light.radius * light.radius;
    float cos = std::abs(dot(light.direction, S.direction));
    float sa = A * cos / lightDistSq;
    S.L = E * sa;

    return S;
}

inline RadianceRGB Renderer::sampleAllDiskLights(const Scene & scene,
                                                 RNG & rng,
                                                 const BRDF & brdf,
                                                 const Direction3 & Wo,
                                                 const Position3 & P,
                                                 const Direction3 & N,
                                                 float minDistance) const
{
    RadianceRGB Lo;

    // Sample disk lights
    for(const auto & light : scene.diskLights) {
        LightSample S = sampleDiskLight(scene, rng, light, P, N, epsilon);
        float F = brdf.eval(Wo, S.direction, N);
        Lo += F * S.L * clampedDot(S.direction, N);
    }

    return Lo;
}

inline bool Renderer::intersectsScene(const Scene & scene,
                                      RNG & rng,
                                      const Ray & ray,
                                      float minDistance,
                                      float maxDistance) const
{
    RayIntersection intersection;
    float A = 1.0f;
    bool hit = false;

    // Check for intersection with the scene. If we hit a transparent material, keep trying.
    do {
        hit = findIntersectionWorldRay(ray, scene, minDistance, intersection);

        if(hit && intersection.distance > maxDistance) {
            return false;
        }

        // Transparency
        if(hit) {
            const Material & material = materialFromID(intersection.material, scene.materials);
            A = material.alpha(scene.textureCache.textures, intersection.texcoord);
            minDistance = intersection.distance + epsilon;
        }
    } while(hit && A < 1.0f && rng.uniform01() > A);

    return hit;
}

inline RadianceRGB Renderer::sampleEnvironmentMap(
                const Scene & scene, RNG & rng, const BRDF & brdf,
                const Direction3 & Wo, const Position3 & P, const Direction3 & N,
                float minDistance, unsigned int numSamples) const
{
    RadianceRGB Lenv;

    // Importance sample environment map. Note, we do not draw another
    // sample if the sample is not visible, as doing so biases the estimate.
    for(unsigned int envSample = 0; envSample < numSamples; ++envSample) {
        vec2 e = rng.uniform2DRange01();
        RandomDirection dirSample = scene.environmentMap->importanceSampleDirection(e.x, e.y);
        float DdotN = dot(dirSample.direction, N);
        
        if(dirSample.pdf > 0.0f && DdotN > 0.0f) {
            Ray ray{ P + N * epsilon, dirSample.direction };

            bool hit = intersectsScene(scene, rng, ray, minDistance);

            if(!hit) {
                float F = brdf.eval(Wo, dirSample.direction, N);
                RadianceRGB Li = scene.environmentMap->sampleRay(ray); 
                Lenv += F * DdotN * Li / dirSample.pdf;
            }
        }
    }

    return Lenv / float(numSamples);
}

void Renderer::printConfiguration() const
{
    auto onoff = [](bool v) { return v ? "ON" : "OFF"; };

    printf("Renderer Configuration:\n");
    printf("  Epsilon = %.8f\n", epsilon);
    printf("  Max depth = %u\n", maxDepth);
    printf("  Monte Carlo refraction = %s\n", onoff(monteCarloRefraction));
    printf("  Russian Roulette:\n"
           "    Chance = %.2f\n"
           "    Minimum depth = %u\n",
           russianRouletteChance, russianRouletteMinDepth);

    auto & dp = shadeDiffuseParams;
    printf("  Diffuse shading:\n"
           "    Environment map samples = %u\n"
           "    Sample cosine lobe = %s\n"
           "    Sample point lights = %s\n",
           dp.numEnvMapSamples, onoff(dp.sampleCosineLobe), onoff(dp.sampleLights));

    auto & sp = shadeSpecularParams;
    printf("  Specular shading:\n"
           "    Sample Phong lobe = %s\n",
           onoff(sp.samplePhongLobe));
}

void Renderer::logConfiguration(Logger & logger) const
{
    auto onoff = [](bool v) { return v ? "ON" : "OFF"; };

    logger.normalf("Renderer Configuration:");
    logger.normalf("  Epsilon = %.8f", epsilon);
    logger.normalf("  Max depth = %u", maxDepth);
    logger.normalf("  Monte Carlo refraction = %s", onoff(monteCarloRefraction));
    logger.normalf("  Russian Roulette:");
    logger.normalf("    Chance = %.2f", russianRouletteChance);
    logger.normalf("    Minimum depth = %u", russianRouletteMinDepth);

    auto & dp = shadeDiffuseParams;
    logger.normalf("  Diffuse shading:");
    logger.normalf("    Environment map samples = %u", dp.numEnvMapSamples);
    logger.normalf("    Sample cosine lobe = %s", onoff(dp.sampleCosineLobe));
    logger.normalf("    Sample point lights = %s", onoff(dp.sampleLights));

    auto & sp = shadeSpecularParams;
    logger.normalf("  Specular shading:");
    logger.normalf("    Sample Phong lobe = %s", onoff(sp.samplePhongLobe));
}
