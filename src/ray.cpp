#include <iostream>
#include "ray.h"

std::ostream & operator<<(std::ostream & os, const Ray & r)
{
    const char * sep = ", ";
    os << "Ray origin " << r.origin << " direction " << r.direction;
    return os;
}


