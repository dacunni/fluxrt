#include "material.h"
#include "renderer.h"
#include "ray.h"
#include "rng.h"
#include "scene.h"

void printDepthPrefix(unsigned int num)
{
    for(unsigned int i = 0; i < num; ++i) {
        printf("  ");
    }
}

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
    const bool hasDiffuse = material.hasDiffuse();
    const bool hasSpecular = material.hasSpecular();
    const bool isRefractive = material.isRefractive;
    const float nMaterial = material.indexOfRefraction;

    RadianceRGB Ld, Ls, Lt;

    auto traceReflect = [&]() {
        RayIntersection reflectIntersection;
        traceRay(scene, rng, Ray(P + N * epsilon, mirror(Wi, N)),
                 epsilon, depth + 1, iorStack, reflectIntersection, Ls);
    };

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

                auto traceRefract = [&]() {
                    RayIntersection refractIntersection;
                    traceRay(scene, rng, Ray(P - N * epsilon, d),
                             epsilon, depth + 1, nextIorStack, refractIntersection, Lt);
                };

                if(monteCarloRefraction) {
                    // Randomly choose a reflected or refracted ray using Fresnel as the
                    // weighting factor
                    if(F == 1.0f || rng.uniform01() < F) { traceReflect(); }
                    else                                 { traceRefract(); }
                }
                else {
                    traceReflect();
                    traceRefract();
                    // Apply Fresnel
                    Ls = F * Ls;
                    Lt = (1.0f - F) * Lt;
                }
            }
        }
    }
    else {
        // Trace diffuse bounce
        if(hasDiffuse) {
            Direction3 diffuseDir(rng.cosineAboutDirection(N));
            RayIntersection nextIntersection;
            traceRay(scene, rng,
                     // Sample according to cosine lobe about the normal
                     Ray(P + N * epsilon, diffuseDir),
                     epsilon, depth + 1, iorStack, nextIntersection, Ld);

            // Sample point lights
            for(const auto & light: scene.pointLights) {
                Direction3 toLight = (light.position - P);
                if(dot(toLight, intersection.normal) < 0.0f)
                    continue;
                Direction3 lightDir = toLight.normalized();
                float lightDist = toLight.magnitude();

                if(!intersects(Ray{P, lightDir}, scene, epsilon, lightDist)) {
                    float lightDistSq = toLight.magnitude_sq();
                    float c = clampedDot(lightDir, intersection.normal);
                    RadianceRGB Ld_point = light.intensity * c / lightDistSq;
                    Ld += Ld_point;
                }
            }
        }

        // Trace specular bounce
        if(hasSpecular) {
            traceReflect();
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

