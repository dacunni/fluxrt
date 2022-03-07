#ifndef __VALUERGB_H__
#define __VALUERGB_H__

#include <string>

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

    bool hasNonZeroComponent() const { return r > 0.0f || g > 0.0f || b > 0.0f; }

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

// Subtypes

struct ColorRGB : public ValueRGB<ColorRGB, ValueRGBInitZero>
{
    ColorRGB()                           : ValueRGB()        {}
    ColorRGB(float r, float g, float b)  : ValueRGB(r, g, b) {}
    ColorRGB(float rgb[3])               : ValueRGB(rgb)     {}

    std::string string() const;
};

struct RadianceRGB : public ValueRGB<RadianceRGB, ValueRGBInitZero>
{
    RadianceRGB()                          : ValueRGB()        {}
    RadianceRGB(float r, float g, float b) : ValueRGB(r, g, b) {}
    RadianceRGB(float rgb[3])              : ValueRGB(rgb)     {}
};

inline RadianceRGB operator+(const RadianceRGB & a, const RadianceRGB & b)
    { return { a.r + b.r, a.g + b.g, a.b + b.b }; }
inline RadianceRGB operator+=(RadianceRGB & a, const RadianceRGB & b)
    { a = a + b; return a; }

inline RadianceRGB operator*(float s, const RadianceRGB & r)
    { return { s * r.r, s * r.g, s * r.b }; }
inline RadianceRGB operator*(const RadianceRGB & r, float s)
    { return operator*(s, r); }
inline RadianceRGB operator*=(RadianceRGB & r, float s)
    { r = r * s; return r; }

inline RadianceRGB operator/(const RadianceRGB & r, float s)
    { return { r.r / s, r.g / s, r.b / s }; }
inline RadianceRGB operator/=(RadianceRGB & r, float s)
    { r = r / s; return r; }

struct ReflectanceRGB : public ValueRGB<ReflectanceRGB, ValueRGBInitOne>
{
    ReflectanceRGB(float r, float g, float b)   : ValueRGB(r, g, b) {}
    ReflectanceRGB(float rgb[3])                : ValueRGB(rgb) {}
    explicit ReflectanceRGB(const ColorRGB & c) : ValueRGB(c) {}
};

inline RadianceRGB operator*(const ReflectanceRGB & ref,
                             const RadianceRGB & rad);

struct ParameterRGB : public ValueRGB<ParameterRGB, ValueRGBInitZero>
{
    ParameterRGB(float r, float g, float b)   : ValueRGB(r, g, b) {}
    ParameterRGB(float rgb[3])                : ValueRGB(rgb) {}
    explicit ParameterRGB(const ColorRGB & c) : ValueRGB(c) {}
};

inline RadianceRGB operator*(const ParameterRGB & param,
                             const RadianceRGB & rad);
inline RadianceRGB operator*(const RadianceRGB & rad,
                             const ParameterRGB & param);

// Inline implementations

inline RadianceRGB operator*(const ReflectanceRGB & ref,
                             const RadianceRGB & rad) {
    return { ref.r * rad.r, ref.g * rad.g, ref.b * rad.b };
}

inline RadianceRGB operator*(const ParameterRGB & param,
                             const RadianceRGB & rad) {
    return { param.r * rad.r, param.g * rad.g, param.b * rad.b };
}

inline RadianceRGB operator*(const RadianceRGB & rad,
                             const ParameterRGB & param) {
    return operator*(param, rad);
}

#endif
