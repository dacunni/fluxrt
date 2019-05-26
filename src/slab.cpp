#include <algorithm>
#include <vector>
#include <cmath>
#include "slab.h"

Slab Slab::centeredUnitCube()
{
    return centeredCube(1.0f);
}

Slab Slab::centeredCube(float size)
{
    float s = size * 0.5f;
    return { -s, s, -s, s, -s, s };
}

void Slab::correctMinMax(void)
{
    if(xmin > xmax) { std::swap(xmin, xmax); }
    if(ymin > ymax) { std::swap(ymin, ymax); }
    if(zmin > zmax) { std::swap(zmin, zmax); }
}

float Slab::maxdim() const
{
    return std::max({xdim(), ydim(), zdim()});
}

void Slab::print() const
{
    printf("Slab min [%f %f %f] max [%f %f %f] dim [%f %f %f]\n",
           xmin, ymin, zmin, xmax, ymax, zmax,
           xdim(), ydim(), zdim());
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

Slab merge(const Slab & a, const Position3 & p)
{
    return Slab(std::min(a.xmin, p.x),
                std::min(a.ymin, p.y),
                std::min(a.zmin, p.z),
                std::max(a.xmax, p.x),
                std::max(a.ymax, p.y),
                std::max(a.zmax, p.z));
}

Slab boundingBox(const std::vector<Position3> & points)
{
    if(points.empty()) {
        return Slab();
    }

    Slab bounds(points[0], points[0]);

    for(auto & p : points) {
        bounds = merge(bounds, p);
    }

    return bounds;
}

vec3 relativeScale(const Slab & a, const Slab & b)
{
    return { b.xdim() / a.xdim(),
             b.ydim() / a.ydim(),
             b.zdim() / a.zdim() };
}

