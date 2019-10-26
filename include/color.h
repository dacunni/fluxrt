#ifndef __COLOR__
#define __COLOR__

#include <limits>
#include <string>
#include <algorithm>

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

struct ColorRGB
{
    ColorRGB() : r(0.0f), g(0.0f), b(0.0f) {}
    ColorRGB(float r, float g, float b) : r(r), g(g), b(b) {}
    ColorRGB(const ColorRGB & c) : r(c.r), g(c.g), b(c.b) {}
    ~ColorRGB() = default;

    static const ColorRGB WHITE()   { return { 1.0f, 1.0f, 1.0f }; }
    static const ColorRGB BLACK()   { return { 0.0f, 0.0f, 0.0f }; }
    static const ColorRGB RED()     { return { 1.0f, 0.0f, 0.0f }; }
    static const ColorRGB GREEN()   { return { 0.0f, 1.0f, 0.0f }; }
    static const ColorRGB BLUE()    { return { 0.0f, 0.0f, 1.0f }; }
    static const ColorRGB CYAN()    { return { 0.0f, 1.0f, 1.0f }; }
    static const ColorRGB MAGENTA() { return { 1.0f, 0.0f, 1.0f }; }
    static const ColorRGB YELLOW()  { return { 1.0f, 1.0f, 0.0f }; }

    std::string string() const;

    float r, g, b;
};

}; // namespace color

#endif
