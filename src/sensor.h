#ifndef __SENSOR_H__
#define __SENSOR_H__

#include <cstdint>
#include <functional>
#include <vector>
#include <thread>
#include <list>
#include <future>

#include <generator.h>

#include "interpolation.h"
#include "vectortypes.h"
#include "vec2.h"

class Logger;

using ThreadIndex = uint32_t;

struct Sensor
{
	inline Sensor() = default;
	Sensor(uint32_t pixelwidth, uint32_t pixelheight);
	inline ~Sensor() = default;

    using PixelFunction = std::function<void(size_t /*x*/, size_t /*y*/, ThreadIndex)>;

    // Call a function for every pixel on the sensor
    void forEachPixel(const PixelFunction & fn);
    void forEachPixelInRect(const PixelFunction & fn,
                            size_t xmin, size_t ymin,
                            size_t xdim, size_t ydim);
    void forEachPixelTiled(const PixelFunction & fn, uint32_t tileSize);
    void forEachPixelThreaded(const PixelFunction & fn, uint32_t numThreads);
    void forEachPixelTiledThreaded(const PixelFunction & fn, uint32_t tileSize,
                                   uint32_t numThreads);

    // Standard image location ranges from x in [-1,+1], y in [-1,+1],
    // regardless of actual aspect ratio.
    inline vec2 pixelStandardImageLocation(float x, float y);
    inline vec2 pixelStandardImageLocation(const vec2 & v);

    inline float aspectRatio() const { return float(pixelwidth) / float(pixelheight); }

    void print() const;

    void logSummary(Logger & logger) const;

    uint32_t pixelwidth = 1;
    uint32_t pixelheight = 1;
};


// Inline Definitions

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
