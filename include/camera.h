#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "vectortypes.h"
#include "ray.h"

struct Camera
{
	Camera() = default;
	Camera(const Position3 & p, const Direction3 & d, const Direction3 & u);
	virtual ~Camera() = default;

    void print() const;

    // Ray passing through a standardized image plane that always
    // ranges from x in [-1,+1], y in [-1,+1], regardless of actual
    // aspect ratio.
    virtual Ray rayThroughStandardImagePlane(float x, float y) const = 0;

    Position3 position;
    Direction3 direction;
    //Direction3 lookat;
    Direction3 up{ 0.0f, 1.0f, 0.0f };
    Direction3 right{ 1.0f, 0.0f, 0.0f };
};

struct PinholeCamera : public Camera
{
    PinholeCamera();
    PinholeCamera(float hfov, float vfov);
	virtual ~PinholeCamera() = default;

    virtual Ray rayThroughStandardImagePlane(float x, float y) const override;

    float hfov = 45.0f * M_PI / 180.0f;
    float vfov = 45.0f * M_PI / 180.0f;

    // Precalculated values
    float hfovOverTwo;
    float vfovOverTwo;

    float tanHfovOverTwo;
    float tanVfovOverTwo;
};

struct OrthoCamera : public Camera
{
    OrthoCamera() = default;
    OrthoCamera(float hsize, float vsize) : hsize(hsize), vsize(vsize) {}
    virtual ~OrthoCamera() = default;

    virtual Ray rayThroughStandardImagePlane(float x, float y) const override;

    float hsize = 2.0f;
    float vsize = 2.0f;
};

#endif
