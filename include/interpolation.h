#ifndef __INTERPOLATION_H__
#define __INTERPOLATION_H__

#include <cmath>

template <typename T> inline T lerpImpl(T a, T v1, T v2)  { return (T(1.0) - a) * v1 + a * v2; }

template <typename T> inline T lerp(float a, T v1, T v2)  { return (T) lerpImpl<float>(a, float(v1), float(v2)); }
template <typename T> inline T lerp(double a, T v1, T v2) { return (T) lerpImpl<double>(a, double(v1), double(v2)); }

template <> inline int32_t lerp(float a, int32_t v1, int32_t v2) { return (int32_t) lerpImpl<double>(double(a), double(v1), double(v2)); }
template <> inline int32_t lerp(double a, int32_t v1, int32_t v2) { return (int32_t) lerpImpl<double>(a, double(v1), double(v2)); }
template <> inline uint32_t lerp(float a, uint32_t v1, uint32_t v2) { return (uint32_t) lerpImpl<double>(double(a), double(v1), double(v2)); }
template <> inline uint32_t lerp(double a, uint32_t v1, uint32_t v2) { return (uint32_t) lerpImpl<double>(a, double(v1), double(v2)); }

template <typename T> inline T bilerp(float a, float b, T v11, T v12, T v21, T v22) {
    return lerp(a, lerp(b, v11, v12), lerp(b, v21, v22));
}

template <typename T> inline T lerpFromTo(float from, T fromMin, T fromMax, T toMin, T toMax) {
    float d = float(fromMax) - float(fromMin);
    if(d == 0.0f) {
        return toMin;
    }
    float a = (float(from) - float(fromMin)) / d;
    return lerp(a, toMin, toMax);
}

#endif
