#include <cmath>
#include "jacobian.h"

namespace jacobian {

namespace differential {

// Calculates the Jacobian (area change factor) given 2D
// differentials of uv -> xy mapping x(u,v) and y(u,v).
// Calculated as:
//
//        | dx/du  dx/dv |
//   det( |              | )
//        | dy/du  dy/dv |
//
float from2Dto2D(float dxdu, float dxdv, float dydu, float dydv)
{
    return std::fabs(dxdu * dydv - dxdv * dydu);
}

// Calculates the Jacobian (volume change factor) given 3D
// differentials of uvw -> xyz mapping x(u,v,w), y(u,v,w) and z(u,v,w).
// Calculated as:
//
//        | dx/du  dx/dv  dx/dw |
//        |                     |
//   det( | dy/du  dy/dv  dy/dw | )
//        |                     |
//        | dz/du  dz/dv  dz/dw |
//
float from3Dto3D(float dxdu, float dxdv, float dxdw,
                 float dydu, float dydv, float dydw,
                 float dzdu, float dzdv, float dzdw)
{
    return std::fabs(
        dxdu * (dydv * dzdw - dydw * dzdv)
      - dxdv * (dydu * dzdw - dydw * dzdu)
      + dxdw * (dydu * dzdv - dydv * dzdu)
    );
}

}; // differential

namespace numerical {

}; // numerical


}; // jacobian
