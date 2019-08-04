#ifndef _JACOBIAN_H_
#define _JACOBIAN_H_

namespace jacobian {

namespace differential {

float from2Dto2D(float dxdu, float dxdv,
                 float dydu, float dydv);

float from3Dto3D(float dxdu, float dxdv, float dxdw,
                 float dydu, float dydv, float dydw,
                 float dzdu, float dzdv, float dzdw);

}; // differential

namespace numerical {

using mappingUVtoScalar = float (*)(float, float);
using mappingUVWtoScalar = float (*)(float, float, float);

float from2Dto2D(mappingUVtoScalar x,
                 mappingUVtoScalar y,
                 float u, float v,
                 float du, float dv);

float from3Dto3D(mappingUVWtoScalar x,
                 mappingUVWtoScalar y,
                 mappingUVWtoScalar z,
                 float u, float v, float w,
                 float du, float dv, float dw);


}; // numerical

}; // jacobian

#endif
