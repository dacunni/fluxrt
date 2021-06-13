#include "traceable.h"
#include "slab.h"

Slab Traceable::boundingBoxTransformed()
{
    auto bounds = boundingBox();
    auto corners = bounds.corners();

    for(auto & corner : corners) {
        corner = transform.fwd * corner;
    }

    return ::boundingBox(corners);
}

