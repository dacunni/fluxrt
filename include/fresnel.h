#ifndef _FRESNEL_H_
#define _FRESNEL_H_

struct ReflectanceRGB;


namespace fresnel {

float schlick(float F0, float cos_i);
ReflectanceRGB schlick(const ReflectanceRGB & F0, float cos_i);

namespace dialectric {

// Fresnel formula for reflectance of a dialectric (non-conductive) material
// (eg: glass), with explicit incident and transmittance angles.
float unpolarized(float cos_i, float cos_t, float n_i, float n_t);
float parallel(float cos_i, float cos_t, float n_i, float n_t);
float perpendicular(float cos_i, float cos_t, float n_i, float n_t);

// Fresnel formula for reflectance of a dialectric (non-conductive) material
// (eg: glass), with explicit incident angle (transmittance derived from
// Snell's Law).
float unpolarizedSnell(float cos_i, float n_i, float n_t);
float parallelSnell(float cos_i, float n_i, float n_t);
float perpendicularSnell(float cos_i, float n_i, float n_t);

float normal(float n_i, float n_t);

}; // dialectric

namespace conductor {

float unpolarized(float cos_i, float n, float k);

float normal(float n_i, float n_t, float k_t);

}; // conductor

}; // fresnel

#endif
