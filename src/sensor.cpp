#include <cstdio>
#include "sensor.h"

Sensor::Sensor(uint32_t pixelwidth, uint32_t pixelheight)
    : pixelwidth(pixelwidth), pixelheight(pixelheight)
{}

void Sensor::print() const
{
    printf("Sensor: %u x %u\n", pixelwidth, pixelheight);
}

