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
        bool traceRay(const Scene & scene, RNG & rng, const Ray & ray, const float minDistance, const unsigned int depth,
                      const MediumStack & mediumStack,
                      RayIntersection & intersection, RadianceRGB & Lo) const;

        RadianceRGB traceRay(const Scene & scene, RNG & rng, const Ray & ray, const float minDistance, const unsigned int depth,
                      const MediumStack & mediumStack) const;

        bool traceCameraRay(const Scene & scene, RNG & rng, const Ray & ray, const float minDistance, const unsigned int depth,
                            const MediumStack & mediumStack,
                            RayIntersection & intersection, RadianceRGB & Lo) const;
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

        inline RadianceRGB directLightingAlongRay(const Scene & scene,
                                                  const Ray & ray) const;

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

        const float epsilon = 1.0e-4;
        unsigned int maxDepth = 2;

        // Use Monte Carlo to choose between reflected and transmitted
        // ray at a refraction boundary. If false, both rays are traced.
        bool monteCarloRefraction = true;

        // Russian roulette chance [0, 1]. 0 = no RR termination
        float russianRouletteChance = 0.1f;
};

