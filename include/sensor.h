#ifndef __SENSOR_H__
#define __SENSOR_H__

#include <cstdint>
#include <functional>
#include "interpolation.h"
#include "vectortypes.h"
#include "vec2.h"

struct Sensor
{
	inline Sensor() = default;
	Sensor(uint32_t pixelwidth, uint32_t pixelheight);
	inline ~Sensor() = default;

    using PixelFunction = std::function<void(size_t /*x*/, size_t /*y*/)>;

    // Call a function for every pixel on the sensor
    inline void forEachPixel(const PixelFunction & fn);

    // Standard image location ranges from x in [-1,+1], y in [-1,+1],
    // regardless of actual aspect ratio.
    inline vec2 pixelStandardImageLocation(float x, float y);
    inline vec2 pixelStandardImageLocation(const vec2 & v);

    inline float aspectRatio() const { return float(pixelwidth) / float(pixelheight); }

    void print() const;

    uint32_t pixelwidth = 1;
    uint32_t pixelheight = 1;
};


// Inline Definitions

void Sensor::forEachPixel(const Sensor::PixelFunction & fn)
{
    for(int y = 0; y < pixelheight; y++) {
        for(int x = 0; x < pixelwidth; x++) {
            fn(x, y);
        }
    }
}

inline vec2 Sensor::pixelStandardImageLocation(float x, float y)
{
    // NOTE: We're flipping y so +y is up
    return vec2(lerpFromTo(x, 0.0f, float(pixelwidth), -1.0f, +1.0f),
                lerpFromTo(y, 0.0f, float(pixelheight), +1.0f, -1.0f));
}

inline vec2 Sensor::pixelStandardImageLocation(const vec2 & v)
{
    return pixelStandardImageLocation(v.x, v.y);
}

#endif
