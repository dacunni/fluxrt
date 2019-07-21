#ifndef __SENSOR_H__
#define __SENSOR_H__

#include <cstdint>

struct Sensor
{
	inline Sensor() = default;
	Sensor(uint32_t pixelwidth, uint32_t pixelheight);
	inline ~Sensor() = default;

    void print() const;

    uint32_t pixelwidth = 1;
    uint32_t pixelheight = 1;
};

#endif
