#ifndef __AMBIENTOCCLUSION_H__
#define __AMBIENTOCCLUSION_H__

#include "scene.h"
#include "Ray.h"
#include "rng.h"

float computeAmbientOcclusion(Scene & scene, const RayIntersection & intersection, float minDistance, RNG & rng,
                              unsigned int numSamples, bool sampleCosineLobe)
{
    const float epsilon = 1.0e-4;
    Position3 p = intersection.position + intersection.normal * epsilon;
    Direction3 d;
    float ao = 0.0f;
    for(int i = 0; i < numSamples; ++i) {
        if(sampleCosineLobe) {
            // Sample according to cosine lobe about the normal
            rng.cosineAboutDirection(intersection.normal, d);
            Ray aoShadowRay(p, d);
            ao += intersectsWorldRay(aoShadowRay, scene, minDistance) ? 0.0f : 1.0f;
        }
        else {
            // Sample hemisphere and scale by cosine of angle to normal
            rng.uniformSurfaceUnitHalfSphere(intersection.normal, d);
            Ray aoShadowRay(p, d);
            ao += intersectsWorldRay(aoShadowRay, scene, minDistance) ? 0.0f : 2.0f * dot(d, intersection.normal);
        }
    }
    ao /= numSamples;

    return ao;
}


#endif
