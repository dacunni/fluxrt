#ifndef __Slab__
#define __Slab__

#include "ray.h"

struct Slab
{
    inline Slab() = default;
    inline Slab(float xmind, float ymind, float zmind,
                float xmaxd, float ymaxd, float zmaxd);
    inline Slab(float xmind, float ymind, float zmind, float sz);
    inline Slab(const Position3 & minpos, const Position3 & maxpos);
    inline ~Slab() = default;
    
    // Factories
    static Slab centeredUnitCube();
    static Slab centeredCube(float size);
    
    void correctMinMax(void);

    inline float xdim() const { return std::abs(xmax - xmin); }
    inline float ydim() const { return std::abs(ymax - ymin); }
    inline float zdim() const { return std::abs(zmax - zmin); }
    float maxdim() const;

    inline float xmid() const { return 0.5f * (xmin + xmax); }
    inline float ymid() const { return 0.5f * (ymin + ymax); }
    inline float zmid() const { return 0.5f * (zmin + zmax); }
    inline Position3 midpoint() const { return Position3{ xmid(), ymid(), zmid() }; }

    inline float volume() const { return xdim() * ydim() * zdim(); }

    void print() const;

    float xmin = 0.0f, ymin = 0.0f, zmin = 0.0f;
    float xmax = 0.0f, ymax = 0.0f, zmax = 0.0f;
};

Slab merge(const Slab & a, const Slab & b);
Slab merge(const Slab & a, const Position3 & p);

Slab boundingBox(const std::vector<Position3> & points);

vec3 relativeScale(const Slab & a, const Slab & b);

// Ray intersection
inline bool intersects(const Ray & ray, const Slab & slab, float minDistance);
inline bool findIntersection(const Ray & ray, const Slab & slab, float minDistance, RayIntersection & intersection);

#include "slab.hpp"
#endif
