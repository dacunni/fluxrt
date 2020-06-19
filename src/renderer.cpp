#include "material.h"
#include "renderer.h"
#include "ray.h"
#include "rng.h"
#include "scene.h"
#include "coordinate.h"

void printDepthPrefix(unsigned int num)
{
    for(unsigned int i = 0; i < num; ++i) {
        printf("  ");
    }
}

bool Renderer::traceRay(const Scene & scene, RNG & rng, const Ray & ray, const float minDistance, const unsigned int depth,
                        const MediumStack & mediumStack,
                        RayIntersection & intersection, RadianceRGB & Lo) const
{
    if(depth > maxDepth) {
        return false;
    }

    bool hit = findIntersection(ray, scene, minDistance, intersection);

    if(!hit) {
        Lo = scene.environmentMap->sampleRay(ray);
        return false;
    }

    const Direction3 Wi = -ray.direction;

    const Material & material = materialFromID(intersection.material, scene.materials);
    auto A = material.alpha(scene.textureCache.textures, intersection.texcoord);

    if(material.hasNormalMap()) {
        auto normalMap = material.normalMap(scene.textureCache.textures, intersection.texcoord);

        auto normal = (intersection.normal * normalMap.z +
                       intersection.tangent * normalMap.x +
                       intersection.bitangent * normalMap.y).normalized();
        auto tangent = cross(intersection.bitangent, normal).normalized();
        auto bitangent = cross(normal, tangent).normalized();
        
        intersection.normal = normal;
        intersection.tangent = tangent;
        intersection.bitangent = bitangent;
    }

    if(A < 1.0f) { // transparent
        // Trace a new ray just past the intersection
        float newMinDistance = intersection.distance + epsilon;
        return traceRay(scene, rng, ray, newMinDistance, depth, mediumStack, intersection, Lo);
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

    return true;
}

RadianceRGB Renderer::traceRay(const Scene & scene, RNG & rng, const Ray & ray, const float minDistance, const unsigned int depth,
                               const MediumStack & mediumStack) const
{
    RayIntersection intersection;
    RadianceRGB L;
    
    bool hit = traceRay(scene, rng, ray, minDistance, depth, mediumStack, intersection, L);

    return L;
}

inline RadianceRGB Renderer::shade(const Scene & scene, RNG & rng, const float minDistance, const unsigned int depth,
                                   const MediumStack & mediumStack,
                                   const Direction3 & Wi, RayIntersection & intersection, const Material & material) const
{
    // Notational convenience
    const auto P = intersection.position;
    auto N = intersection.normal;

    if(dot(Wi, N) < 0.0f) {
        N.negate();
    }

    const auto D  = material.diffuse(scene.textureCache.textures, intersection.texcoord);
    const auto S  = material.specular(scene.textureCache.textures, intersection.texcoord);
    const bool hasDiffuse = material.hasDiffuse();
    const bool hasSpecular = material.hasSpecular();
    const bool isRefractive = material.isRefractive;
    const Medium & medium = material.innerMedium;

    RadianceRGB Ld, Ls, Lr;

    if(isRefractive) {
        // Refraction
        Lr = shadeRefractiveInterface(scene, rng, minDistance, depth, mediumStack, medium, Wi, P, N);
    }
    else {
        // Trace diffuse bounce
        if(hasDiffuse) {
            Ld = shadeDiffuse(scene, rng, minDistance, depth, mediumStack, Wi, P, N);
        }

        // Trace specular bounce
        if(hasSpecular) {
            Ls = shadeReflect(scene, rng, minDistance, depth, mediumStack, Wi, P, N);
        }
    }

    RadianceRGB Lo;

    if(isRefractive) {
        Lo = Lr;
    }
    else if(hasSpecular) {
        // Fresnel = specular
        ReflectanceRGB F0rgb = S;
        float cosIncidentAngle = absDot(Wi, N);
        ReflectanceRGB Frgb = fresnel::schlick(F0rgb, cosIncidentAngle);
        ReflectanceRGB OmFrgb = Frgb.residual();
        Lo = OmFrgb * (D * Ld) + Frgb * Ls;
    }
    else {
        Lo = D * Ld;
    }

    return Lo;
}

bool Renderer::traceCameraRay(const Scene & scene, RNG & rng, const Ray & ray, const float minDistance, const unsigned int depth,
                              const MediumStack & mediumStack,
                              RayIntersection & intersection, RadianceRGB & Lo) const
{
    bool hit = traceRay(scene, rng, ray, minDistance, depth, mediumStack, intersection, Lo);

    Lo += directLightingAlongRay(scene, ray);

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
    RadianceRGB L;

    L += traceRay(scene, rng, ray, epsilon, depth + 1, mediumStack);
    L += directLightingAlongRay(scene, ray);

    return L;
}


inline RadianceRGB Renderer::shadeRefract(const Scene & scene, RNG & rng,
                                          const float minDistance, const unsigned int depth,
                                          const MediumStack & mediumStack,
                                          const Direction3 & Dt,
                                          const Position3 & P, const Direction3 & N) const
{
    const Ray ray(P - N * epsilon, Dt);
    RadianceRGB L;

    L += traceRay(scene, rng, ray, epsilon, depth + 1, mediumStack);
    L += directLightingAlongRay(scene, ray);

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

inline RadianceRGB Renderer::directLightingAlongRay(const Scene & scene,
                                                    const Ray & ray) const
{
    RayIntersection lightIntersection;
    RadianceRGB L;

    // Check for hit on disk lights
    for(const auto & light : scene.diskLights) {
        // Skip if we don't hit the light
        if(!findIntersection(ray, light, epsilon, lightIntersection))
            continue;
        // Make sure we don't hit an object closer than the light
        if(!intersects(ray, scene, epsilon, lightIntersection.distance)) {
            L += light.intensity / (lightIntersection.distance * lightIntersection.distance);
        }
    }

    return L;
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

#if 1
    // Sample according to cosine lobe about the normal
    Direction3 diffuseDir(rng.cosineAboutDirection(N));
    L += traceRay(scene, rng, Ray(P + N * epsilon, diffuseDir),
                  epsilon, depth + 1, mediumStack);
#else
    // Uniform sampling across the hemisphere
    Direction3 diffuseDir(rng.uniformSurfaceUnitHalfSphere(N));
    L += 2.0f * clampedDot(diffuseDir, N)
        * traceRay(scene, rng, Ray(P + N * epsilon, diffuseDir),
                   epsilon, depth + 1, mediumStack);
#endif

    L += sampleDirectLighting(scene, rng, P, N);

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
    if(intersects(Ray{P, lightDir}, scene, minDistance, lightDist)) {
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
    if(intersects(Ray{P, lightDir}, scene, epsilon, lightDist)) {
        return RadianceRGB::BLACK();
    }

    return light.intensity  / lightDistSq;
}


