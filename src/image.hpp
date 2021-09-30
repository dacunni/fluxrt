#include <cassert>
#include <cmath>
#include "color.h"
#include "interpolation.h"

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
    else if(outOfBoundsBehavior == Repeat) {
        x = (x % width + width) % width;
        y = (y % height + height) % height;
    }
    return data[index(x, y, channel)];
}

template<typename T>
inline ColorRGB Image<T>::getRGB(size_t x, size_t y) const
{
    return {
        get(x, y, 0),
        get(x, y, 1),
        get(x, y, 2)
    };
}

template<typename T>
inline T Image<T>::channelSum(size_t x, size_t y) const
{
    T sum = (T) 0;
    for(int c = 0; c < numChannels; ++c) {
        sum += get(x, y, c);
    }
    return sum;
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
    else if(outOfBoundsBehavior == Repeat) {
        x = std::fmod(std::fmod(x, width) + width, width);
        y = std::fmod(std::fmod(y, height) + height, height);
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
inline T Image<T>::lerpUV(float u, float v, int channel) const
{
    return lerp(u * width, v * height, channel);
}

template<typename T>
inline ColorRGB Image<T>::lerpUV3(float u, float v) const
{
    return {
        lerpUV(u, v, 0),
        lerpUV(u, v, 1),
        lerpUV(u, v, 2)
    };
}

template<typename T>
inline void Image<T>::setAll(T value)
{
    std::fill(data.begin(), data.end(), value);
}

template<typename T>
inline void Image<T>::accum(size_t x, size_t y, int channel, T value)
{
    data[index(x, y, channel)] += value;
}

template<typename T>
inline void Image<T>::accum(size_t x, size_t y, const ColorRGB & c)
{
    data[index(x, y, 0)] += c.r;
    data[index(x, y, 1)] += c.g;
    data[index(x, y, 2)] += c.b;
}

template<typename T>
inline void Image<T>::accum3(size_t x, size_t y, T v0, T v1, T v2)
{
    data[index(x, y, 0)] += v0;
    data[index(x, y, 1)] += v1;
    data[index(x, y, 2)] += v2;
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

template<typename T>
inline void Image<T>::set3(size_t x, size_t y, const ColorRGB & c)
{
    set3(x, y, c.r, c.g, c.b);
}

template<typename T>
void Image<T>::forEachPixel(const PixelFunction & fn)
{
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            fn(*this, x, y);
        }
    }
}

template<typename T>
void Image<T>::forEachPixelChannel(const PixelChannelFunction & fn)
{
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            for(int c = 0; c < numChannels; c++) {
                fn(*this, x, y, c);
            }
        }
    }
}

template<typename T>
void Image<T>::forEachPixelInRow(size_t y, const PixelFunction & fn)
{
    for(int x = 0; x < width; x++) {
        fn(*this, x, y);
    }
}

template<typename T>
void Image<T>::forEachPixelInColumn(size_t x, const PixelFunction & fn)
{
    for(int y = 0; y < height; y++) {
        fn(*this, x, y);
    }
}

namespace testpattern {

template<typename T>
Image<T> grayRamp(size_t xsize, size_t ysize)
{
    using namespace color::channel;
    const auto high = maxValue<T>();
    Image<T> image(xsize, ysize, 3);

    for(int y = 0; y < ysize; ++y) {
        for(int x = 0; x < xsize; ++x) {
            T value = (T) (float(x) / float(xsize - 1) * high);
            image.set3(x, y, value, value, value);
        }
    }

    return image;
}

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

template<typename T>
Image<T> checkerBoardBlackAndWhite(size_t w, size_t h,
                                   size_t checkSize, int numChannels)
{
    const T minValue = color::channel::minValue<T>();
    const T maxValue = color::channel::maxValue<T>();

    Image<T> image(w, h, numChannels);

    image.setAll(minValue);

    for(int y = 0; y < h; y++) {
        for(int x = 0; x < w; x++) {
            if(((x / checkSize) & 0x1) ^ ((y / checkSize) & 0x1)) {
                for(int c = 0; c < numChannels; c++) {
                    image.set(x, y, c, maxValue);
                }
            }
        }
    }

    return image;
}

}; // namespace testpattern
