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

float from2Dto2D(mappingUVtoScalar x,
                 mappingUVtoScalar y,
                 float u, float v,
                 float du, float dv)
{
    const float x_uv = x(u, v);
    const float y_uv = y(u, v);

    const float un = u + du;
    const float vn = v + dv;

    float dxdu = (x(un, v ) - x_uv) / du;
    float dxdv = (x(u , vn) - x_uv) / dv;
    float dydu = (y(un, v ) - y_uv) / du;
    float dydv = (y(u , vn) - y_uv) / dv;

    return differential::from2Dto2D(dxdu, dxdv, dydu, dydv);
}

float from3Dto3D(mappingUVWtoScalar x,
                 mappingUVWtoScalar y,
                 mappingUVWtoScalar z,
                 float u, float v, float w,
                 float du, float dv, float dw)
{
    const float x_uvw = x(u, v, w);
    const float y_uvw = y(u, v, w);
    const float z_uvw = z(u, v, w);

    const float un = u + du;
    const float vn = v + dv;
    const float wn = w + dw;

    float dxdu = (x(un, v , w ) - x_uvw) / du;
    float dxdv = (x(u , vn, w ) - x_uvw) / dv;
    float dxdw = (x(u , v , wn) - x_uvw) / dw;
    float dydu = (y(un, v , w ) - y_uvw) / du;
    float dydv = (y(u , vn, w ) - y_uvw) / dv;
    float dydw = (y(u , v , wn) - y_uvw) / dw;
    float dzdu = (y(un, v , w ) - z_uvw) / du;
    float dzdv = (y(u , vn, w ) - z_uvw) / dv;
    float dzdw = (y(u , v , wn) - z_uvw) / dw;

    return differential::from3Dto3D(dxdu, dxdv, dxdw,
                                    dydu, dydv, dydw,
                                    dzdu, dzdv, dzdw);
}

}; // numerical


}; // jacobian
