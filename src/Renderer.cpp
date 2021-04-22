#include "material.h"
#include "Renderer.h"
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

    bool hit = findIntersectionWorldRay(ray, scene, minDistance, intersection);

    if(!hit) {
        if(accumEnvMap) {
            Lo = scene.environmentMap->sampleRay(ray);
        }
        return false;
    }

    const Direction3 Wi = -ray.direction;

    const Material & material = materialFromID(intersection.material, scene.materials);
    auto A = material.alpha(scene.textureCache.textures, intersection.texcoord);

    material.applyNormalMap(scene.textureCache.textures, intersection.texcoord,
                            intersection.normal, intersection.tangent, intersection.bitangent);

    // Transparency
    if(A < 1.0f && rng.uniform01() > A) {
        // Trace a new ray just past the intersection
        float newMinDistance = intersection.distance + epsilon;
        return traceRay(scene, rng, ray, newMinDistance, depth, mediumStack, accumEmission, accumEnvMap, intersection, Lo);
    }

    Lo = shade(scene, rng, minDistance, depth, mediumStack, Wi, intersection, material);

    // Apply Beer's Law attenuation
    ParameterRGB att = mediumStack.back().beersLawAttenuation;
    ParameterRGB beer {
        optics::beersLawAttenuation(att.r, intersection.distance),
        optics::beersLawAttenuation(att.g, intersection.distance),
        optics::beersLawAttenuation(att.b, intersection.distance)
    };
    Lo = Lo * beer;

    // Emission
    if(accumEmission) {
        const auto E = material.emission(scene.textureCache.textures, intersection.texcoord);
        Lo += E;
    }

    // Account for RR loss
    if(depth >= russianRouletteMinDepth) {
        Lo /= (1.0f - russianRouletteChance);
    }

    return true;
}

RadianceRGB Renderer::traceRay(const Scene & scene, RNG & rng,
                               const Ray & ray,
                               const float minDistance, const unsigned int depth,
                               const MediumStack & mediumStack,
                               bool accumEmission, bool accumEnvMap) const
{
    RayIntersection intersection;
    RadianceRGB L;
    
    bool hit = traceRay(scene, rng, ray, minDistance, depth, mediumStack, accumEmission, accumEnvMap, intersection, L);

    return L;
}

inline RadianceRGB Renderer::shade(const Scene & scene, RNG & rng,
                                   const float minDistance,
                                   const unsigned int depth,
                                   const MediumStack & mediumStack,
                                   const Direction3 & Wi,
                                   RayIntersection & intersection,
                                   const Material & material) const
{
    // Notational convenience
    const auto P = intersection.position;
    auto N = intersection.normal;

    if(dot(Wi, N) < 0.0f) {
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

    if(material.isRefractive) {
        Lo = shadeRefractiveInterface(scene, rng, minDistance, depth, mediumStack, medium, Wi, P, N);
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
            F = fresnel::schlick(F0, absDot(Wi, N));
        }

        // Trace specular bounce
        if(doSpec) {
            if(material.isGlossy()) {
                Ls = shadeSpecularGlossy(scene, rng, minDistance, depth, mediumStack, Wi, P, N, material.specularExponent);
            }
            else {
                Ls = shadeReflect(scene, rng, minDistance, depth, mediumStack, Wi, P, N);
            }
            Ls /= probSpec;
        }

        // Trace diffuse bounce
        if(doDiffuse) {
            Ld = shadeDiffuse(scene, rng, minDistance, depth, mediumStack, Wi, P, N);
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

    return hit;
}

inline RadianceRGB Renderer::shadeReflect(const Scene & scene, RNG & rng,
                                          const float minDistance, const unsigned int depth,
                                          const MediumStack & mediumStack,
                                          const Direction3 & Wi,
                                          const Position3 & P, const Direction3 & N) const
{
    const Direction3 Wo = mirror(Wi, N);
    const Ray ray(P + N * epsilon, Wo);
    return traceRay(scene, rng, ray, epsilon, depth + 1, mediumStack, true, true);
}


inline RadianceRGB Renderer::shadeRefract(const Scene & scene, RNG & rng,
                                          const float minDistance, const unsigned int depth,
                                          const MediumStack & mediumStack,
                                          const Direction3 & Dt,
                                          const Position3 & P, const Direction3 & N) const
{
    const Ray ray(P - N * epsilon, Dt);
    RadianceRGB L;

    L += traceRay(scene, rng, ray, epsilon, depth + 1, mediumStack, true, true);

    return L;
}

inline RadianceRGB Renderer::shadeRefractiveInterface(const Scene & scene, RNG & rng,
                                                      const float minDistance, const unsigned int depth,
                                                      const MediumStack & mediumStack,
                                                      const Medium & medium,
                                                      const Direction3 & Wi,
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

    Direction3 d = refract(Wi, N, n1, n2);

    bool totalInternalReflection = d.isZeros();

    if(totalInternalReflection) {
        // Reflected ray
        Ls = shadeReflect(scene, rng, minDistance, depth, mediumStack, Wi, P, N);
    }
    else {
        float F = fresnel::dialectric::unpolarized(dot(Wi, N), dot(d, -N), n1, n2);

        if(monteCarloRefraction) {
            // Randomly choose a reflected or refracted ray using Fresnel as the
            // weighting factor
            if(F == 1.0f || rng.uniform01() < F) {
                Ls = shadeReflect(scene, rng, minDistance, depth, mediumStack, Wi, P, N);
            }
            else {
                Lt = shadeRefract(scene, rng, minDistance, depth, nextMediumStack, d, P, N);
            }
        }
        else {
            Ls = shadeReflect(scene, rng, minDistance, depth, mediumStack, Wi, P, N);
            Lt = shadeRefract(scene, rng, minDistance, depth, nextMediumStack, d, P, N);
            // Apply Fresnel
            Ls = F * Ls;
            Lt = (1.0f - F) * Lt;
        }
    }

    return Ls + Lt;
}

inline RadianceRGB Renderer::sampleDirectLighting(const Scene & scene,
                                                  RNG & rng,
                                                  const Position3 & P,
                                                  const Direction3 & N) const
{
    RadianceRGB L;
    Direction3 lightDir;

    // Sample point lights
    for(const auto & light: scene.pointLights) {
        RadianceRGB Lp = samplePointLight(scene, light, P, N, epsilon, lightDir);
        float c = clampedDot(lightDir, N);
        L += c * Lp;
    }

    // Sample disk lights
    for(const auto & light : scene.diskLights) {
        RadianceRGB Lp = sampleDiskLight(scene, rng, light, P, N, epsilon, lightDir);
        float c = clampedDot(lightDir, N);
        L += c * Lp;
    }

    return L;
}

inline RadianceRGB Renderer::shadeDiffuse(const Scene & scene, RNG & rng,
                                          const float minDistance, const unsigned int depth,
                                          const MediumStack & mediumStack,
                                          const Direction3 & Wi,
                                          const Position3 & P, const Direction3 & N) const
{
    RadianceRGB L;

    // Note: We don't accumulate emission on the next hit if we sample
    //       lighting directly, to avoid double counting direct illumination.

    const bool sampleEnvMap = scene.environmentMap->canImportanceSample();

    if(shadeDiffuseParams.sampleLights) {
        L += sampleDirectLighting(scene, rng, P, N) / constants::PI;
    }

    if(sampleEnvMap) {
        RadianceRGB Lenv;

        for(unsigned int envSample = 0; envSample < shadeDiffuseParams.numEnvMapSamples; ++envSample) {
            // Importance sample environment map. Note, we do not draw another
            // sample if the sample is not visible, as doing so biases the estimate.
            vec2 e = rng.uniform2DRange01();
            RandomDirection dirSample = scene.environmentMap->importanceSampleDirection(e.x, e.y);
            float DdotN = dot(dirSample.direction, N);

            if(dirSample.pdf > 0.0f && DdotN > 0.0f) {
                Ray r{ P + N * epsilon, dirSample.direction };
                if(!intersectsWorldRay(r, scene, minDistance)) {
                    Lenv += DdotN * scene.environmentMap->sampleRay(r) / dirSample.pdf;
                }
            }
        }

        L += Lenv / float(shadeDiffuseParams.numEnvMapSamples);
    }

    if(shadeDiffuseParams.sampleCosineLobe) {
        // Sample according to cosine lobe about the normal
        Direction3 diffuseDir(rng.cosineAboutDirection(N));
        L += traceRay(scene, rng, Ray(P + N * epsilon, diffuseDir),
                      epsilon, depth + 1, mediumStack,
                      !shadeDiffuseParams.sampleLights,
                      !sampleEnvMap);
    }
    else {
        // Uniform sampling across the hemisphere
        Direction3 diffuseDir(rng.uniformSurfaceUnitHalfSphere(N));
        L += 2.0f * clampedDot(diffuseDir, N)
            * traceRay(scene, rng, Ray(P + N * epsilon, diffuseDir),
                       epsilon, depth + 1, mediumStack,
                       !shadeDiffuseParams.sampleLights,
                       !sampleEnvMap);
    }

    return L;
}

inline RadianceRGB Renderer::shadeSpecularGlossy(const Scene & scene, RNG & rng,
                                                 const float minDistance, const unsigned int depth,
                                                 const MediumStack & mediumStack,
                                                 const Direction3 & Wi,
                                                 const Position3 & P, const Direction3 & N,
                                                 float exponent) const
{
    RadianceRGB L;

    if(shadeSpecularParams.samplePhongLobe) {
        // Importance sample the Phong distribution
        Direction3 mirrorDir = mirror(Wi, N);
        Direction3 tangent, bitangent;
        coordinate::coordinateSystem(mirrorDir, tangent, bitangent);

        vec2 u = rng.uniform2DRange01();
        float theta = std::acos(std::pow(u.x, 1.0f / (exponent + 1.0f)));
        float phi = u.y * constants::TWO_PI;
        float sin_theta = std::sin(theta);

        Direction3 Wo = std::cos(theta) * mirrorDir +
            sin_theta * std::cos(phi) * tangent +
            sin_theta * std::sin(phi) * bitangent;

        if(dot(Wo, N) > 0.0f) {
            L += traceRay(scene, rng, Ray(P + N * epsilon, Wo),
                          epsilon, depth + 1, mediumStack, true, true);
        }
    }
    else {
        // Uniform sampling across the hemisphere
        Direction3 Wo(rng.uniformSurfaceUnitHalfSphere(N));

        // Evaluate BRDF
        L += constants::TWO_PI * brdf::phong(Wi, Wo, N, exponent)
            * traceRay(scene, rng, Ray(P + N * epsilon, Wo),
                       epsilon, depth + 1, mediumStack, true, true);
    }

    return L;
}

inline RadianceRGB Renderer::samplePointLight(const Scene & scene,
                                              const PointLight & light,
                                              const Position3 & P,
                                              const Direction3 & N,
                                              float minDistance,
                                              Direction3 & lightDir) const
{
    Direction3 toLight = light.position - P;

    // Make sure the light is on the right size of the surface
    if(dot(toLight, N) < 0.0f)
        return RadianceRGB::BLACK();

    lightDir = toLight.normalized();
    const float lightDistSq = toLight.magnitude_sq();
    const float lightDist = std::sqrt(lightDistSq);

    // If we hit something, we can't see the light
    if(intersectsWorldRay(Ray{P, lightDir}, scene, minDistance, lightDist)) {
        return RadianceRGB::BLACK();
    }

    return light.intensity / lightDistSq;
}

inline RadianceRGB Renderer::sampleDiskLight(const Scene & scene,
                                             RNG & rng,
                                             const DiskLight & light,
                                             const Position3 & P,
                                             const Direction3 & N,
                                             float minDistance,
                                             Direction3 & lightDir) const
{
    vec2 offset = rng.uniformCircle(light.radius);
    // rotate to align with direction
    vec3 ax1, ax2;
    coordinate::coordinateSystem(light.direction, ax1, ax2);
    vec3 pointOnDisk = offset.x * ax1 + offset.y * ax2;
    Position3 pointOnLight = light.position + Direction3(pointOnDisk);
    Direction3 toLight = pointOnLight - P;

    // Make sure the light is on the right size of the surface
    if(dot(toLight, N) < 0.0f)
        return RadianceRGB::BLACK();

    lightDir = toLight.normalized();
    const float lightDistSq = toLight.magnitude_sq();
    const float lightDist = std::sqrt(lightDistSq);

    // If we hit something, we can't see the light
    if(intersectsWorldRay(Ray{P, lightDir}, scene, epsilon, lightDist - epsilon)) {
        return RadianceRGB::BLACK();
    }

    const Material & material = materialFromID(light.material, scene.materials);
    RayIntersection lightIntersection; // FIXME - dummy for texcoords that we don't support yet
    auto E = material.emission(scene.textureCache.textures, lightIntersection.texcoord);

    // Scale by the solid angle of the light as seen by the shaded point
    float A = constants::PI * light.radius * light.radius;
    float cos = std::abs(dot(light.direction, lightDir));
    float sa = A * cos / lightDistSq;
    return E * sa;
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

