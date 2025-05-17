#ifndef _MICROFACET_H_
#define _MICROFACET_H_

#include <stdexcept>

namespace Microfacet {

namespace GeometryShadowing {

enum FunctionSelector { IMPLICIT, COOK_TORRANCE };

inline float Function(FunctionSelector selector, float NdH, float NdV, float NdL, float VdH);
inline float Implicit(float NdV, float NdL);
inline float CookTorrance(float NdH, float NdV, float NdL, float VdH);

}; // GeometryShadowing

namespace NormalDistribution {

enum FunctionSelector { BECKMAN, BLINN_PHONG };

inline float Function(FunctionSelector selector, float roughness, float NdH);
inline float Beckman(float roughness, float NdH);
inline float BlinnPhong(float roughness, float NdH);

}; // NormalDistribution

// Implementations

namespace GeometryShadowing {

inline float Function(FunctionSelector selector, float NdH, float NdV, float NdL, float VdH)
{
    switch(selector) {
        case IMPLICIT:      return Implicit(NdV, NdL); break;
        case COOK_TORRANCE: return CookTorrance(NdH, NdV, NdL, VdH); break;
        default:
            throw std::runtime_error("Unknown microfacet geometry shadowing function selector");
    }
}

inline float Implicit(float NdV, float NdL)
{
    return NdV * NdL;
}

inline float CookTorrance(float NdH, float NdV, float NdL, float VdH)
{
    float G1 = 2.0f * NdH * NdV / VdH;
    float G2 = 2.0f * NdH * NdL / VdH;
    return std::min(1.0f, std::min(G1, G2));
}

}; // GeometryShadowing

namespace NormalDistribution {

inline float Function(FunctionSelector selector, float roughness, float NdH)
{
    switch(selector) {
        case BECKMAN:     return Beckman(roughness, NdH); break;
        case BLINN_PHONG: return BlinnPhong(roughness, NdH); break;
        default:
            throw std::runtime_error("Unknown microfacet normal distribution function selector");
    }
}

inline float Beckman(float roughness, float NdH)
{
    float m2 = roughness * roughness;
    float NdH2 = NdH * NdH;
    float NdH4 = NdH2 * NdH2;

    //assert(roughness > 0.0f && m2 > 0.0f && NdH2 > 0.0f && NdH4 > 0.0f);

    float x = (NdH2 - 1) / (m2 * NdH2);
    float num = std::exp(x);
    float den = M_PI * m2 * NdH4;

    return num / den;
}

inline float BlinnPhong(float roughness, float NdH)
{
    float m2 = roughness * roughness;
    float NdH2 = NdH * NdH;

    //assert(m2 > 0.0f);

    float c = 1.0f / (m2 * M_PI);
    float e = 2 / m2 - 2;
    float D = c * std::pow(NdH, e);

    return D;
}

}; // NormalDistribution

}; // Microfacet


#endif
