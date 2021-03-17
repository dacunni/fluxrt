#include "fresnel.h"
#include "radiometry.h"

struct Direction3;
struct Material;
struct Position3;
struct Ray;
struct RayIntersection;
struct RNG;
struct Scene;
struct PointLight;
struct DiskLight;

class Renderer
{
    using MediumStack = std::vector<Medium>;

    public:
        bool traceRay(const Scene & scene, RNG & rng,
                      const Ray & ray,
                      const float minDistance, const unsigned int depth,
                      const MediumStack & mediumStack,
                      bool accumEmission,
                      bool accumEnvMap,
                      RayIntersection & intersection,
                      RadianceRGB & Lo) const;

        RadianceRGB traceRay(const Scene & scene, RNG & rng,
                             const Ray & ray,
                             const float minDistance, const unsigned int depth,
                             const MediumStack & mediumStack,
                             bool accumEmission,
                             bool accumEnvMap) const;

        bool traceCameraRay(const Scene & scene, RNG & rng, const Ray & ray, const float minDistance, const unsigned int depth,
                            const MediumStack & mediumStack,
                            RayIntersection & intersection, RadianceRGB & Lo) const;

        void printConfiguration() const;

    protected:

        inline RadianceRGB shade(const Scene & scene, RNG & rng, const float minDistance, const unsigned int depth,
                                 const MediumStack & mediumStack,
                                 const Direction3 & Wi, RayIntersection & intersection, const Material & material) const;

        inline RadianceRGB shadeReflect(const Scene & scene, RNG & rng,
                                        const float minDistance, const unsigned int depth,
                                        const MediumStack & mediumStack,
                                        const Direction3 & Wi,
                                        const Position3 & P, const Direction3 & N) const;

        inline RadianceRGB shadeRefract(const Scene & scene, RNG & rng,
                                        const float minDistance, const unsigned int depth,
                                        const MediumStack & mediumStack,
                                        const Direction3 & Dt,
                                        const Position3 & P, const Direction3 & N) const;

        inline RadianceRGB shadeRefractiveInterface(const Scene & scene, RNG & rng,
                                                    const float minDistance, const unsigned int depth,
                                                    const MediumStack & mediumStack,
                                                    const Medium & medium,
                                                    const Direction3 & Wi,
                                                    const Position3 & P, const Direction3 & N) const;

        inline RadianceRGB shadeDiffuse(const Scene & scene, RNG & rng,
                                        const float minDistance, const unsigned int depth,
                                        const MediumStack & mediumStack,
                                        const Direction3 & Wi,
                                        const Position3 & P, const Direction3 & N) const;

        inline RadianceRGB shadeSpecularGlossy(const Scene & scene, RNG & rng,
                                               const float minDistance, const unsigned int depth,
                                               const MediumStack & mediumStack,
                                               const Direction3 & Wi,
                                               const Position3 & P, const Direction3 & N,
                                               float exponent) const;

        inline RadianceRGB sampleDirectLighting(const Scene & scene,
                                                RNG & rng,
                                                const Position3 & P,
                                                const Direction3 & N) const;

        inline RadianceRGB samplePointLight(const Scene & scene,
                                            const PointLight & light,
                                            const Position3 & P,
                                            const Direction3 & N,
                                            float minDistance,
                                            Direction3 & lightDir) const;

        inline RadianceRGB sampleDiskLight(const Scene & scene,
                                           RNG & rng,
                                           const DiskLight & light,
                                           const Position3 & P,
                                           const Direction3 & N,
                                           float minDistance,
                                           Direction3 & lightDir) const;

    public:

        static constexpr float        DEFAULT_EPSILON   = 1.0e-4;
        static constexpr unsigned int DEFAULT_MAX_DEPTH = 10;

        float epsilon = DEFAULT_EPSILON;

        unsigned int maxDepth = DEFAULT_MAX_DEPTH;

        // Use Monte Carlo to choose between reflected and transmitted
        // ray at a refraction boundary. If false, both rays are traced.
        bool monteCarloRefraction = true;

        // Russian roulette chance [0, 1]. 0 = no RR termination
        float russianRouletteChance = 0.1f;
        // Only apply RR if at or beyond this depth
        unsigned int russianRouletteMinDepth = 2;

        // Diffuse shading parameters
        struct {
            unsigned int numEnvMapSamples = 10;
            bool sampleCosineLobe = true;
            bool sampleLights = true;
        } shadeDiffuseParams;

        // Specular shading parameters
        struct {
            bool samplePhongLobe = true;
        } shadeSpecularParams;
};

