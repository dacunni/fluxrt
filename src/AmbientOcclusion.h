#ifndef __AMBIENTOCCLUSION_H__
#define __AMBIENTOCCLUSION_H__

#include "scene.h"
#include "Ray.h"
#include "rng.h"

float computeAmbientOcclusion(Scene & scene, const RayIntersection & intersection, float minDistance, RNG & rng,
                              unsigned int numSamples, bool sampleCosineLobe);

#endif
