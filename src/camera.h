#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "vectortypes.h"
#include "vec2.h"
#include "Ray.h"
#include "constants.h"

struct Camera
{
	Camera() = default;
	Camera(const Position3 & p, const Direction3 & d, const Direction3 & u);
	virtual ~Camera() = default;

    void print() const;

    // Ray passing through a standardized image plane that always
    // ranges from x in [-1,+1], y in [-1,+1], regardless of actual
    // aspect ratio. Blur coordinates should be random points within
    // a unit circle.
    virtual Ray rayThroughStandardImagePlane(float x, float y, float blurx, float blury) const = 0;

    Ray rayThroughStandardImagePlane(const vec2 & v, const vec2 & blur) const
        { return rayThroughStandardImagePlane(v.x, v.y, blur.x, blur.y); }

    void setPositionDirectionUp(const Position3 & p, const Direction3 & d, const Direction3 & u);

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

    virtual Ray rayThroughStandardImagePlane(float x, float y, float blurx, float blury) const override;

    float hfov = DegreesToRadians(45.0f);
    float vfov = DegreesToRadians(45.0f);

    bool applyLensBlur = false;
    float focusDistance = 10.0f;
    float focusDivergence = 0.01f;

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

    virtual Ray rayThroughStandardImagePlane(float x, float y, float blurx, float blury) const override;

    float hsize = 2.0f;
    float vsize = 2.0f;
};

#endif
