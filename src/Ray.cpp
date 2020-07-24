#include <iostream>
#include "Ray.h"

std::ostream & operator<<(std::ostream & os, const Ray & r)
{
    os << "Ray origin " << r.origin << " direction " << r.direction;
    return os;
}

std::ostream & operator<<(std::ostream & os, const RayIntersection & ri)
{
    os << "RayIntersection\n"
        << "   ray { origin " << ri.ray.origin << ", direction " << ri.ray.direction << " }\n"
        << "   positon " << ri.position << '\n'
        << "   normal " << ri.normal << '\n'
        << "   tangent " << ri.tangent << '\n'
        << "   bitagent " << ri.bitangent << '\n'
        << "   distance " << ri.distance << '\n'
        << "   texcoord " << ri.texcoord << " has " << ri.hasTexCoord << '\n';

    return os;
}

