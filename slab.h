#ifndef __Slab__
#define __Slab__

struct Slab
{
    inline Slab() = default;
    inline Slab(float xmind, float ymind, float zmind,
                float xmaxd, float ymaxd, float zmaxd);
    inline Slab(float xmind, float ymind, float zmind, float sz);
    inline ~Slab() = default;
    
    void correctMinMax( void );

    inline float xdim() const { return std::abs(xmax - xmin); }
    inline float ydim() const { return std::abs(ymax - ymin); }
    inline float zdim() const { return std::abs(zmax - zmin); }
    float maxdim() const;

    inline float xmid() const { return 0.5f * (xmin + xmax); }
    inline float ymid() const { return 0.5f * (ymin + ymax); }
    inline float zmid() const { return 0.5f * (zmin + zmax); }

    inline float volume() const { return xdim() * ydim() * zdim(); }
    
    float xmin = 0.0f, ymin = 0.0f, zmin = 0.0f;
    float xmax = 0.0f, ymax = 0.0f, zmax = 0.0f;
};

Slab merge(const Slab & a, const Slab & b);

#include "slab.hpp"
#endif
