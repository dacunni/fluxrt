#ifndef _RADIOMETRY_H_
#define _RADIOMETRY_H_

#include "ValueRGB.h"
#include "ValueArray.h"

// Radiometric units

// Wavelength (m)
using Wavelength = float;

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

// Utility functions
Irradiance irradianceAtDistanceFromPointLight(Power power, float radius);

#endif
