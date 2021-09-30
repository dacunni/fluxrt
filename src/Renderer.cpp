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

    const Direction3 Wo = -ray.direction;

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
            if(material.isGlossy()) {
                Ls = shadeSpecularGlossy(scene, rng, minDistance, depth, mediumStack, Wo, P, N, material.specularExponent);
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

    return hit;
}

inline RadianceRGB Renderer::shadeReflect(const Scene & scene, RNG & rng,
                                          const float minDistance, const unsigned int depth,
                                          const MediumStack & mediumStack,
                                          const Direction3 & Wo,
                                          const Position3 & P, const Direction3 & N) const
{
    const Direction3 Wi = mirror(Wo, N);
    const Ray ray(P + N * epsilon, Wi);
    return traceRay(scene, rng, ray, epsilon, depth + 1, mediumStack, true, true);
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
    RadianceRGB L;

    // Note: We don't accumulate emission on the next hit if we sample
    //       lighting directly, to avoid double counting direct illumination.

    const bool sampleEnvMap = scene.environmentMap->canImportanceSample();

    if(shadeDiffuseParams.sampleLights) {
        // Sample point lights
        for(const auto & light: scene.pointLights) {
            LightSample S = samplePointLight(scene, light, P, N, epsilon);
            float F = brdf::lambertian(Wo, S.direction, N);
            L += F * S.L * clampedDot(S.direction, N);
        }

        // Sample disk lights
        for(const auto & light : scene.diskLights) {
            LightSample S = sampleDiskLight(scene, rng, light, P, N, epsilon);
            float F = brdf::lambertian(Wo, S.direction, N);
            L += F * S.L * clampedDot(S.direction, N);
        }
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

    brdfSample S;

    if(shadeDiffuseParams.sampleCosineLobe) {
        // Sample according to cosine lobe about the normal
        S.W = Direction3(rng.cosineAboutDirection(N));
        S.pdf = clampedDot(S.W, N) / constants::PI;
        // FIXME - looks right, but can we avoid NaNs by not
        //         computing dot / dot?
    }
    else {
        // Uniform sampling across the hemisphere
        S.W = Direction3(rng.uniformSurfaceUnitHalfSphere(N));
        S.pdf = 1.0f / constants::TWO_PI;
    }

    float F = brdf::lambertian(Wo, S.W, N);
    L += F / S.pdf * clampedDot(S.W, N)
        * traceRay(scene, rng, Ray(P + N * epsilon, S.W),
                   epsilon, depth + 1, mediumStack,
                   !shadeDiffuseParams.sampleLights,
                   !sampleEnvMap);

    return L;
}

inline RadianceRGB Renderer::shadeSpecularGlossy(const Scene & scene, RNG & rng,
                                                 const float minDistance, const unsigned int depth,
                                                 const MediumStack & mediumStack,
                                                 const Direction3 & Wo,
                                                 const Position3 & P, const Direction3 & N,
                                                 float exponent) const
{
    RadianceRGB L;
    brdfSample S;

    if(shadeSpecularParams.samplePhongLobe) {
        // Importance sample the Phong distribution
        S = brdf::samplePhong(rng.uniform2DRange01(), Wo, N, exponent);
    }
    else {
        // Uniform sampling across the hemisphere
        S.W = Direction3(rng.uniformSurfaceUnitHalfSphere(N));
        S.pdf = 1.0f / constants::TWO_PI;
    }

    // Evaluate BRDF
    if(dot(S.W, N) > 0.0f) {
        float F = brdf::phong(Wo, S.W, N, exponent);
        float D = clampedDot(S.W, N);
        L += F * D / S.pdf * traceRay(scene, rng, Ray(P + N * epsilon, S.W),
                                      epsilon, depth + 1, mediumStack, true, true);
    }

    return L;
}

inline LightSample Renderer::samplePointLight(const Scene & scene,
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
    if(intersectsWorldRay(Ray{P, S.direction}, scene, minDistance, lightDist)) {
        return S;
    }

    S.L = light.intensity / lightDistSq;

    return S;
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
    if(intersectsWorldRay(Ray{P, S.direction}, scene, epsilon, lightDist - epsilon)) {
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

