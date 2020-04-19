#ifndef __COLOR__
#define __COLOR__

#include <limits>
#include <string>
#include <algorithm>

#include "valuergb.h"

namespace color {

namespace channel {

template<typename T> T minValue() { return (T) 0; }
template<typename T> T maxValue() { return std::numeric_limits<T>::max(); }
template<typename T> T midValue() { return maxValue<T>() / T(2); }

template<> float minValue();
template<> float maxValue();

template<> double minValue();
template<> double maxValue();

template<typename T> T clamp(T value) {
    return std::max(minValue<T>(), std::min(maxValue<T>(), value));
}

}; // namespace channel

struct ColorRGB : public ValueRGB<ColorRGB, ValueRGBInitZero>
{
    ColorRGB()                           : ValueRGB()        {}
    ColorRGB(float r, float g, float b)  : ValueRGB(r, g, b) {}
    ColorRGB(float rgb[3])               : ValueRGB(rgb)     {}

    std::string string() const;
};

}; // namespace color

#endif
