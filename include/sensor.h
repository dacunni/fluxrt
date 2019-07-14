#ifndef __SENSOR_H__
#define __SENSOR_H__

#include <cstdint>

struct Sensor
{
	inline Sensor() = default;
	Sensor(uint32_t width, uint32_t height);
	inline ~Sensor() = default;

    void print() const;

    uint32_t width = 1;
    uint32_t height = 1;
};

#endif
