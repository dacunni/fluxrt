#include <cmath>
#include "camera.h"
#include "constants.h"
#include "interpolation.h"
#include "Logger.h"

Camera::Camera(const Position3 & p, const Direction3 & d, const Direction3 & u)
{
    setPositionDirectionUp(p, d, u);
}

void Camera::setPositionDirectionUp(const Position3 & p,
                                    const Direction3 & d,
                                    const Direction3 & u)
{
    position = p;
    direction = d.normalized();
    right = cross(direction, u).normalized();
    if(right.magnitude() < 0.99) {
        // We were given an invalid up direction. Pick another one arbitrarily
        // by swizzling and negating some of the components of u.
        right = cross(direction, Direction3(-u.y, -u.z, u.x)).normalized();
    }
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

void PinholeCamera::logSummary(Logger & logger) const
{
    logger.normalf("PinholeCamera:");
    logger.normalf("  FOV h=%f v=%f", hfov, vfov);
    logger.normalf("  lens blur=%s focus distance=%f divergence=%f",
        Logger::yesno(applyLensBlur).c_str(),
        focusDistance, focusDivergence
    );
}

Ray PinholeCamera::rayThroughStandardImagePlane(float x, float y, float blurx, float blury) const
{
    // Calculate world offsets
    const float xw = x * tanHfovOverTwo;
    const float yw = y * tanVfovOverTwo;

    auto d = direction + right * xw + up * yw;
    d.normalize();

    Ray ray(position, d);

    // Apply focus blur
    // We offset the ray origin according to a random direction on a plane
    // parallel to the image plane, scaled by the distance to the focal plane
    // and a divergence parameter that describes how much a ray diverges from
    // its origin per unit distance. The result is that points at the focal
    // distance are in focus.
    if(applyLensBlur) {
        // Find the point where the unblurred ray hits the focal plane
        Position3 pointOnFocalPlane = ray.pointAt(focusDistance);

        // Scale the blur coordinate by the focus distance and divergence parameter
        vec2 focusOffset = vec2(blurx, blury) * (focusDistance * focusDivergence);

        // Apply the offset along the plane through the origin parallel to the image plane
        ray.origin += focusOffset.x * right + focusOffset.y * up;

        // Compute the new ray direction as the vector from offset origin point
        // through the original image plane point
        ray.direction = (pointOnFocalPlane - ray.origin).normalized();
    }

    return ray;
}


void OrthoCamera::logSummary(Logger & logger) const
{
    logger.normalf("OrthoCamera:");
    logger.normalf("  size h=%f v=%f", hsize, vsize);
}

Ray OrthoCamera::rayThroughStandardImagePlane(float x, float y, float blurx, float blury) const
{
    float xw = lerpFromTo(x, -1.0f, +1.0f, -0.5f * hsize, 0.5f * hsize);
    float yw = lerpFromTo(y, -1.0f, +1.0f, -0.5f * vsize, 0.5f * vsize);
    auto dpos = right * xw + up * yw;
    return Ray(Position3(position + dpos), direction);
}

