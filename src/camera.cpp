#include <cmath>
#include "camera.h"
#include "constants.h"
#include "interpolation.h"

Camera::Camera(const Position3 & p, const Direction3 & d, const Direction3 & u)
    : position(p), direction(d), up(u)
{
    direction.normalize();
    right = cross(direction, up).normalized();
    up = cross(right, direction).normalized();
}

PinholeCamera::PinholeCamera()
    : PinholeCamera(DegreesToRadians(45.0),
                    DegreesToRadians(45.0))
{
}

PinholeCamera::PinholeCamera(float hfov, float vfov)
    : hfov(hfov), vfov(vfov),
    hfovOverTwo(hfov * 0.5f),
    vfovOverTwo(vfov * 0.5f),
    tanHfovOverTwo(std::tan(hfovOverTwo)),
    tanVfovOverTwo(std::tan(vfovOverTwo))
{
}

Ray PinholeCamera::rayThroughStandardImagePlane(float x, float y) const
{
    // Calculate world offsets
    const float xw = x * std::tan(hfovOverTwo);
    const float yw = y * std::tan(vfovOverTwo);

    auto d = direction + right * xw + up * yw;
    d.normalize();

    return Ray(position, d);
}

Ray OrthoCamera::rayThroughStandardImagePlane(float x, float y) const
{
    float xw = lerpFromTo(x, -1.0f, +1.0f, -0.5f * hsize, 0.5f * hsize);
    float yw = lerpFromTo(y, -1.0f, +1.0f, -0.5f * vsize, 0.5f * vsize);
    return Ray(Position3(position + vec3(xw, yw, 0.0f)), direction);
}

