#include <cmath>
#include <cstdio>
#include "integrate.h"

namespace integrate {

float overUnitHemisphere(std::function<FunctionOverHemisphere> f,
                         unsigned int thetaSteps, unsigned int phiSteps)
{
    const float thetaMax = 0.5 * M_PI;
    const float phiMax   = 2.0 * M_PI;
    const float dtheta   = thetaMax / float(thetaSteps);
    const float dphi     = phiMax / float(phiSteps);

    float sum = 0.0f;

    for(float theta = 0.5f * dtheta; theta < thetaMax; theta += dtheta) {
        float dA = dtheta * dphi * std::sin(theta);
        for(float phi = 0.5f * dphi; phi < phiMax; phi += dphi) {
            sum += f(theta, phi) * dA;
        }
    }

    return sum;
}

float overUnitSphere(std::function<FunctionOverHemisphere> f,
                     unsigned int thetaSteps, unsigned int phiSteps)
{
    const float thetaMax = M_PI;
    const float phiMax   = 2.0 * M_PI;
    const float dtheta   = thetaMax / float(thetaSteps);
    const float dphi     = phiMax / float(phiSteps);

    float sum = 0.0f;

    for(float theta = 0.5f * dtheta; theta < thetaMax; theta += dtheta) {
        float dA = dtheta * dphi * std::sin(theta);
        for(float phi = 0.5f * dphi; phi < phiMax; phi += dphi) {
            sum += f(theta, phi) * dA;
        }
    }

    return sum;
}

}; // integrate

