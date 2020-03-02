#ifndef _RADIOMETRY_H_
#define _RADIOMETRY_H_

// Radiometric units

// Radiant Flux (Phi) or Power (units: Joules/sec, J/s)
//   Energy passing through a surface or region per unit time
using RadiantFlux = float;
using Power = float;

// Irradiance (E) (units: Watts/meter^2, W/m^2)
//   Density of flux per unit area arriving at a surface
using Irradiance = float;

// Radiant Exitance (M)
//   Density of flux per unit area leaving a surface
using RadiantExitance = float;

// Steradian ^ -1
using InverseSteradians = float;

// Radiance
//   Radiant flush per steradian per unit area
using Radiance = float;

// Spectral Radiance
//   Radiance per nanometer wavelength
using SpectralRadiance = float;

// RGB Radiance Samples
struct RadianceRGB
{
    RadianceRGB(float r, float g, float b) : r(r), g(g), b(b) {}
    RadianceRGB() = default;
    ~RadianceRGB() = default;

    Radiance r = 0.0f;
    Radiance g = 0.0f;
    Radiance b = 0.0f;
};

inline RadianceRGB operator+(const RadianceRGB & a, const RadianceRGB & b)
    { return { a.r + b.r, a.g + b.g, a.b + b.b }; }
inline RadianceRGB operator+=(RadianceRGB & a, const RadianceRGB & b)
    { a = a + b; return a; }

inline RadianceRGB operator*(float s, const RadianceRGB & r)
    { return { s * r.r, s * r.g, s * r.b }; }
inline RadianceRGB operator*(const RadianceRGB & r, float s)
    { return operator*(s, r); }
inline RadianceRGB operator*=(RadianceRGB & r, float s)
    { r = r * s; return r; }

inline RadianceRGB operator/(const RadianceRGB & r, float s)
    { return { r.r / s, r.g / s, r.b / s }; }
inline RadianceRGB operator/=(RadianceRGB & r, float s)
    { r = r / s; return r; }

// Utility functions
Irradiance irradianceAtDistanceFromPointLight(Power power, float radius);

#endif
