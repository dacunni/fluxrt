#include "fresnel.h"
#include "radiometry.h"

struct Direction3;
struct Material;
struct Ray;
struct RayIntersection;
struct RNG;
struct Scene;

class Renderer
{
    using RadianceRGB = radiometry::RadianceRGB;
    using IndexOfRefractionStack = std::vector<float>;

    public:
        bool traceRay(const Scene & scene, RNG & rng, const Ray & ray, const float minDistance, const unsigned int depth,
                      const IndexOfRefractionStack & iorStack,
                      RayIntersection & intersection, RadianceRGB & Lo) const;

        radiometry::RadianceRGB shade(const Scene & scene, RNG & rng, const float minDistance, const unsigned int depth,
                                      const IndexOfRefractionStack & iorStack,
                                      const Direction3 & Wi, RayIntersection & intersection, const Material & material) const;

        const float epsilon = 1.0e-4;
        unsigned int maxDepth = 2;
        bool monteCarloRefraction = true;
};
