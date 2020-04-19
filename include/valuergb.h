#ifndef __VALUERGB_H__
#define __VALUERGB_H__

struct ValueRGBInitZero {
    static constexpr float value() { return 0.0f; }
};

struct ValueRGBInitOne {
    static constexpr float value() { return 1.0f; }
};

template <typename T, typename INIT = ValueRGBInitZero>
struct ValueRGB
{
    ValueRGB() = default;
    ValueRGB(float r, float g, float b) : r(r), g(g), b(b) {}
    ValueRGB(float rgb[3]) : r(rgb[0]), g(rgb[1]), b(rgb[2]) {}

    template <typename S>
    explicit ValueRGB(const S & s) : r(s.r), g(s.g), b(s.b) {}

    T residual() const { return { 1.0f - r, 1.0f - g, 1.0f - b }; }

    static const T WHITE()   { return { 1.0f, 1.0f, 1.0f }; }
    static const T BLACK()   { return { 0.0f, 0.0f, 0.0f }; }
    static const T RED()     { return { 1.0f, 0.0f, 0.0f }; }
    static const T GREEN()   { return { 0.0f, 1.0f, 0.0f }; }
    static const T BLUE()    { return { 0.0f, 0.0f, 1.0f }; }
    static const T CYAN()    { return { 0.0f, 1.0f, 1.0f }; }
    static const T MAGENTA() { return { 1.0f, 0.0f, 1.0f }; }
    static const T YELLOW()  { return { 1.0f, 1.0f, 0.0f }; }

    float r = INIT::value();
    float g = INIT::value();
    float b = INIT::value();
};

#endif
