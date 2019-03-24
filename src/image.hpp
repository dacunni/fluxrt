#include <cassert>
#include <cmath>
#include "color.h"

template<typename T>
Image<T>::Image(size_t w, size_t h, int channels)
    : width(w), height(h), numChannels(channels)
{
    data.resize(width * height * numChannels);
}

template<typename T>
inline size_t Image<T>::index(size_t x, size_t y, int channel) const
{
    return (y * width + x) * numChannels + channel;
}

template<typename T>
inline T Image<T>::get(size_t x, size_t y, int channel) const
{
    if(outOfBoundsBehavior == ZeroValueOutOfBounds) {
        if(x < 0 || y < 0 || x >= width || y >= height)
            return T(0);
    }
    else if(outOfBoundsBehavior == ClampOutOfBoundsCoordinate) {
        x = clamp<float>(x, 0, width - 1);
        y = clamp<float>(y, 0, height - 1);
    }
    return data[index(x, y, channel)];
}

template<typename T>
inline T Image<T>::lerp(float x, float y, int channel) const
{
    x -= 0.5f;
    y -= 0.5f;

    if(outOfBoundsBehavior == ZeroValueOutOfBounds) {
        if(x < 0 || y < 0 || x >= width || y >= height)
            return T(0);
    }
    else if(outOfBoundsBehavior == ClampOutOfBoundsCoordinate) {
        x = clamp<float>(x, 0, width - 1);
        y = clamp<float>(y, 0, height - 1);
    }

    // Compute blend factors based on distance of each component
    // from its nearest neighbors.
    float iX, iY;
    float bX = std::modf(x, &iX);
    float bY = std::modf(y, &iY);

    // Get neighbors in 4-connected region
    T v11 = get(iX    , iY    , channel);
    T v12 = get(iX    , iY + 1, channel);
    T v21 = get(iX + 1, iY    , channel);
    T v22 = get(iX + 1, iY + 1, channel);

    // Bilinearly interpolate
    return bilerp(bX, bY, v11, v12, v21, v22);
}

template<typename T>
inline void Image<T>::setAll(T value)
{
    std::fill(data.begin(), data.end(), value);
}

template<typename T>
inline void Image<T>::set(size_t x, size_t y, int channel, T value)
{
    data[index(x, y, channel)] = value;
}

template<typename T>
inline void Image<T>::set3(size_t x, size_t y,
                           T v0, T v1, T v2)
{
    assert(numChannels == 3);
    set(x, y, 0, v0);
    set(x, y, 1, v1);
    set(x, y, 2, v2);
}

namespace testpattern {

template<typename T>
Image<T> colorRange()
{
    using namespace color::channel;
    const auto low = minValue<T>();
    const auto high = maxValue<T>();
    const size_t size = 400;
    Image<T> image(size, size, 3);
    T value;
    auto bandSize = size / 8;
    int yband = 0;

    // gradient
    for(int y = 0; y < bandSize; ++y) {
        for(int x = 0; x < size; ++x) {
            value = (T) (float(x) / float(size - 1) * high);
            image.set3(x, y + yband * bandSize, value, value, value);
        }
    }
    yband++;

    // middle gray and equivalent checkerboard
    auto midOrChecker = [&](int x, int y) {
        if(y > bandSize / 3 && y < 2 * bandSize / 3)
            return (x & 0x1) ^ (y & 0x1) ? low : high;
        else
            return midValue<T>();
    };

    // middle gray and equivalent checkerboard
    for(int y = 0; y < bandSize; ++y) {
        for(int x = 0; x < size; ++x) {
            value = midOrChecker(x, y);
            image.set3(x, y + yband * bandSize, value, value, value);
        }
    }
    yband++;

    auto setChannelMidOrChecker = [&](int channel) {
        for(int y = 0; y < bandSize; ++y) {
            for(int x = 0; x < size; ++x) {
                value = midOrChecker(x, y);
                image.set(x, y + yband * bandSize, channel, value);
            }
        }
    };

    // red
    setChannelMidOrChecker(0);
    yband++;

    // green
    setChannelMidOrChecker(1);
    yband++;

    // blue
    setChannelMidOrChecker(2);
    yband++;

    // cyan
    setChannelMidOrChecker(1);
    setChannelMidOrChecker(2);
    yband++;

    // magenta
    setChannelMidOrChecker(0);
    setChannelMidOrChecker(2);
    yband++;

    // yellow
    setChannelMidOrChecker(0);
    setChannelMidOrChecker(1);
    yband++;

    return image;
}

}; // namespace testpattern
