#include <cstdio>
#include "sensor.h"

Sensor::Sensor(uint32_t width, uint32_t height)
    : width(width), height(height)
{}

void Sensor::print() const
{
    printf("Sensor: %u x %u\n", width, height);
}

