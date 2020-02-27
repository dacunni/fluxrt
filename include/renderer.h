#include "fresnel.h"
#include "radiometry.h"

struct Direction3;
struct Material;
struct Position3;
struct Ray;
struct RayIntersection;
struct RNG;
struct Scene;

class Renderer
{
    using RadianceRGB = radiometry::RadianceRGB;

    struct Medium {
        float indexOfRefraction = 1.0f;
        float beersLawAttenuation = 0.0f;
    };

    using IndexOfRefractionStack = std::vector<float>;

    public:
        bool traceRay(const Scene & scene, RNG & rng, const Ray & ray, const float minDistance, const unsigned int depth,
                      const IndexOfRefractionStack & iorStack,
                      RayIntersection & intersection, RadianceRGB & Lo) const;

        RadianceRGB traceRay(const Scene & scene, RNG & rng, const Ray & ray, const float minDistance, const unsigned int depth,
                      const IndexOfRefractionStack & iorStack) const;

    protected:

        inline RadianceRGB shade(const Scene & scene, RNG & rng, const float minDistance, const unsigned int depth,
                                 const IndexOfRefractionStack & iorStack,
                                 const Direction3 & Wi, RayIntersection & intersection, const Material & material) const;

        inline RadianceRGB shadeReflect(const Scene & scene, RNG & rng,
                                        const float minDistance, const unsigned int depth,
                                        const IndexOfRefractionStack & iorStack,
                                        const Direction3 & Wi,
                                        const Position3 & P, const Direction3 & N) const;

        inline RadianceRGB shadeRefract(const Scene & scene, RNG & rng,
                                        const float minDistance, const unsigned int depth,
                                        const IndexOfRefractionStack & iorStack,
                                        const Direction3 & Dt,
                                        const Position3 & P, const Direction3 & N) const;

        inline RadianceRGB shadeRefractiveInterface(const Scene & scene, RNG & rng,
                                                    const float minDistance, const unsigned int depth,
                                                    const IndexOfRefractionStack & iorStack,
                                                    const float nMaterial,
                                                    const Direction3 & Wi,
                                                    const Position3 & P, const Direction3 & N) const;

        inline RadianceRGB shadeDiffuse(const Scene & scene, RNG & rng,
                                        const float minDistance, const unsigned int depth,
                                        const IndexOfRefractionStack & iorStack,
                                        const Direction3 & Wi,
                                        const Position3 & P, const Direction3 & N) const;

    public:

        const float epsilon = 1.0e-4;
        unsigned int maxDepth = 2;
        bool monteCarloRefraction = true;
};

