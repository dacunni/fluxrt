#ifndef _BRDF_H_
#define _BRDF_H_

#include "radiometry.h"
#include "vec2.h"
#include "vectortypes.h"
#include "rng.h"

struct brdfSample {

    bool isDelta() const { return delta; }

    Direction3 W;
    float pdf;
    bool delta = false;
};

InverseSteradians lambertian(const Direction3 & Wi,
                             const Direction3 & Wo,
                             const Direction3 & N);

InverseSteradians phong(const Direction3 & Wi,
                        const Direction3 & Wo,
                        const Direction3 & N,
                        const float a);       // exponent

// Importance Sampling

brdfSample samplePhong(const vec2 & e,        // random samples
                       const Direction3 & Wi,
                       const Direction3 & N,
                       const float a);        // exponent

class BRDF {
    public:
        BRDF() = default;
        virtual ~BRDF() = default;

        // Evaluate the BRDF
        virtual InverseSteradians eval(const Direction3 & Wi, const Direction3 & Wo, const Direction3 & N) const = 0;

        // Sample Wo from the BRDF, given Wi and random numbers
        virtual brdfSample sample(const vec2 & e, const Direction3 & Wi, const Direction3 & N) = 0;

        // Uniform sampling across the hemisphere
        brdfSample sampleHemisphereUniform(const vec2 & e, const Direction3 & N) {
            brdfSample S;
            S.W = Direction3(RNG::uniformSurfaceUnitHalfSphere(e, N));
            S.pdf = 1.0f / constants::TWO_PI;
            return S;
        }

        bool importanceSample = true;
};

class LambertianBRDF : public BRDF {
    public:
        LambertianBRDF() = default;
        virtual ~LambertianBRDF() = default;
        
        virtual InverseSteradians eval(const Direction3 & Wi, const Direction3 & Wo, const Direction3 & N) const override {
            return lambertian(Wi, Wo, N);
        }

        virtual brdfSample sample(const vec2 & e, const Direction3 & Wi, const Direction3 & N) override {
            if(!importanceSample) {
                return sampleHemisphereUniform(e, N);
            }
            // Sample according to cosine lobe about the normal
            Direction3 Wo = Direction3(RNG::cosineAboutDirection(e, N));
            brdfSample S;
            S.W = Wo;
            S.pdf = clampedDot(S.W, N) / constants::PI;
            return S;
        }
};

class PhongBRDF : public BRDF {
    public:
        PhongBRDF(float a) : a(a) {}
        virtual ~PhongBRDF() = default;
        
        virtual InverseSteradians eval(const Direction3 & Wi, const Direction3 & Wo, const Direction3 & N) const override {
            return phong(Wi, Wo, N, a);
        }

        virtual brdfSample sample(const vec2 & e, const Direction3 & Wi, const Direction3 & N) override {
            if(!importanceSample) {
                return sampleHemisphereUniform(e, N);
            }
            return samplePhong(e, Wi, N, a);
        }

        float a = 0.0f;
};

class MirrorBRDF : public BRDF {
    public:
        MirrorBRDF() {}
        virtual ~MirrorBRDF() = default;
        
        virtual InverseSteradians eval(const Direction3 & Wi, const Direction3 & Wo, const Direction3 & N) const override {
            return 1.0f;
        }

        virtual brdfSample sample(const vec2 & e, const Direction3 & Wi, const Direction3 & N) override {
            brdfSample S;
            S.W = mirror(Wi, N);
            S.pdf = 1.0f;
            S.delta = true;
            return S;
        }
};

#endif
