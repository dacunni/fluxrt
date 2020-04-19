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

        inline RadianceRGB radianceFromPointLight(const Scene & scene,
                                                  const PointLight & light,
                                                  const Position3 & P,
                                                  const Direction3 & N,
                                                  float minDistance,
                                                  Direction3 & lightDir) const;

        inline RadianceRGB radianceFromDiskLight(const Scene & scene,
                                                 RNG & rng,
                                                 const DiskLight & light,
                                                 const Position3 & P,
                                                 const Direction3 & N,
                                                 float minDistance,
                                                 Direction3 & lightDir) const;

    public:

        const float epsilon = 1.0e-4;
        unsigned int maxDepth = 2;
        bool monteCarloRefraction = true;
};

