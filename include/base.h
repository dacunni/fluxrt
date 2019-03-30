#ifndef __BASE_H__
#define __BASE_H__

#include <cmath>
#include <algorithm>

template<typename T> inline T sq(const T & t) { return t * t; }

template <typename T> inline T clamp(T value, const T & low, const T & high) { return std::max(low, std::min(value, high)); }
template <typename T> inline T clamp01(T value) { return clamp<T>(value, 0.0, 1.0); }
template <typename T> inline T clamp0(T value) { return std::max<T>(0.0, value); }

template <typename T> inline T clampedCosine(T angle) { return clamp0(std::cos(angle)); }

#endif
