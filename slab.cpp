#include <algorithm>
#include <cmath>
#include "slab.h"

void Slab::correctMinMax(void)
{
#if 0
    std::tie(xmin, xmax) = std::minmax(xmin, xmax);
    std::tie(ymin, ymax) = std::minmax(ymin, ymax);
    std::tie(zmin, zmax) = std::minmax(zmin, zmax);
#else
    // DAC: Faster in benchmarks than the above
    if(xmin > xmax) { std::swap(xmin, xmax); }
    if(ymin > ymax) { std::swap(ymin, ymax); }
    if(zmin > zmax) { std::swap(zmin, zmax); }
#endif
}

float Slab::maxdim() const
{
    return std::max({xdim(), ydim(), zdim()});
}

Slab merge(const Slab & a, const Slab & b)
{
    return Slab(std::min(a.xmin, b.xmin),
                std::min(a.ymin, b.ymin),
                std::min(a.zmin, b.zmin),
                std::max(a.xmax, b.xmax),
                std::max(a.ymax, b.ymax),
                std::max(a.zmax, b.zmax));
}

