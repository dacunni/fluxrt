#include <cmath>

#include "base.h"
#include "optics.h"
#include "material.h"

namespace fresnel {

// Schlick approximation for Fresnel.
// cos_i = dot(Wi, N) is the cosine of the angle of the incident
// direction and the normal
float schlick(float F0, float cos_i)
{
    return F0 + (1.0f - F0) * std::pow(1.0f - cos_i, 5.0f);
}

// Schlick applied to each component of a RGB reflectance
ReflectanceRGB schlick(const ReflectanceRGB & F0, float cos_i)
{
    float p = std::pow(1.0f - cos_i, 5.0f);
    return {
        F0.r + (1.0f - F0.r) * p,
        F0.g + (1.0f - F0.g) * p,
        F0.b + (1.0f - F0.b) * p,
    };
}

namespace dialectric {

static float sqrtParallel(float cos_i, float cos_t, float n_i, float n_t)
{
    float denom_pa = (n_i * cos_i + n_t * cos_t);
    if(denom_pa == 0.0f) {
        denom_pa = 1.0f; // FIXME?
    }
    return (n_i * cos_i - n_t * cos_t) / denom_pa;
}

float parallel(float cos_i, float cos_t, float n_i, float n_t)
{
    return sq(sqrtParallel(cos_i, cos_t, n_i, n_t));
}

static float sqrtPerpendicular(float cos_i, float cos_t, float n_i, float n_t)
{
    float denom_pe = (n_i * cos_t + n_t * cos_i);
    if(denom_pe == 0.0f) {
        denom_pe = 1.0f; // FIXME?
    }
    return (n_i * cos_t - n_t * cos_i) / denom_pe;
}

float perpendicular(float cos_i, float cos_t, float n_i, float n_t)
{
    return sq(sqrtPerpendicular(cos_i, cos_t, n_i, n_t));
}

// Fresnel formula for reflectance of a dialectric (non-conductive) material
// (eg: glass)
float unpolarized(float cos_i, float cos_t, float n_i, float n_t)
{
    // for unpolarized light, we use the average of parallel and perpendicular terms
    return 0.5f * (parallel(cos_i, cos_t, n_i, n_t) +
                   perpendicular(cos_i, cos_t, n_i, n_t));
}

// Fresnel formula for reflectance of a dialectric (non-conductive) material
// taking Snell's law into account to derive cos_t
float unpolarizedSnell(float cos_i, float n_i, float n_t)
{
    float angle_i = acos(cos_i);
    float angle_t = optics::snellsLawAngle(n_i, angle_i, n_t);
    if(std::isnan(angle_t)) { return 1.0f; }
    return unpolarized(cos_i, cos(angle_t), n_i, n_t);
}

float parallelSnell(float cos_i, float n_i, float n_t)
{
    float angle_i = acos(cos_i);
    float angle_t = optics::snellsLawAngle(n_i, angle_i, n_t);
    if(std::isnan(angle_t)) { return 1.0f; }
    return parallel(cos_i, cos(angle_t), n_i, n_t);
}

float perpendicularSnell(float cos_i, float n_i, float n_t)
{
    float angle_i = acos(cos_i);
    float angle_t = optics::snellsLawAngle(n_i, angle_i, n_t);
    if(std::isnan(angle_t)) { return 1.0f; }
    return perpendicular(cos_i, cos(angle_t), n_i, n_t);
}

float normal(float n_i, float n_t)
{
    float num   = sq(n_t - n_i);
    float denom = sq(n_t + n_i);
    return num / denom;
}

}; // dialectric

namespace conductor {

// Fresnel formula for reflectance of a conductive material (eg: metals)
// (approximate)
float unpolarized(float cos_i, float n, float k)
{
    const float nspks = sq(n) + sq(k);
    const float cs = sq(cos_i);
    const float nspkscs = nspks * cs;
    float twonc = 2.0f * n * cos_i;
    float R_pa2 = (nspkscs - twonc + 1.0f) /
                  (nspkscs + twonc + 1.0f);
    float R_pe2 = (nspks - twonc + cs) /
                  (nspks + twonc + cs);
    return (R_pa2 + R_pe2) * 0.5f;
}

float normal(float n_i, float n_t, float k_t)
{
    float num   = sq(n_t - n_i) + sq(k_t);
    float denom = sq(n_t + n_i) + sq(k_t);
    return num / denom;
}

}; // conductor

}; // fresnel

