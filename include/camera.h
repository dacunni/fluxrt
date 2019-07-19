#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "vectortypes.h"

struct Camera
{
	Camera() = default;
	~Camera() = default;

    void print() const;

    Position3 position;
    //Direction3 lookat;
    Direction3 up{ 0.0f, 1.0f, 0.0f };
};

struct PinholeCamera : public Camera
{
    PinholeCamera() = default;
    PinholeCamera(float hfov)
        : hfov(hfov)
        {}
	~PinholeCamera() = default;

    float hfov = 45.0f;
};

#endif
