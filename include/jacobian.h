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

}; // numerical

}; // jacobian

#endif
