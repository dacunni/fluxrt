#ifndef __BASE_H__
#define __BASE_H__

#include <cmath>
#include <numeric>
#include <algorithm>

template<typename T> inline T sq(const T & t) { return t * t; }

template <typename T> inline T clamp(T value, const T & low, const T & high) { return std::max(low, std::min(value, high)); }
template <typename T> inline T clamp01(T value) { return clamp<T>(value, 0.0, 1.0); }
template <typename T> inline T clamp0(T value) { return std::max<T>(0.0, value); }

template <typename T> inline T clampedCosine(T angle) { return clamp0(std::cos(angle)); }

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

#endif
