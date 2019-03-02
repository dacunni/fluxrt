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
    return data[index(x, y, channel)];
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

template<typename T>
Image<T> applyGamma(const Image<T> & image, float gamma)
{
    Image<T> newImage(image.width, image.height, image.numChannels);

    for(int y = 0; y < image.height; ++y) {
        for(int x = 0; x < image.width; ++x) {
            for(int c = 0; c < image.numChannels; ++c) {
                auto v = image.get(x, y, c);
                newImage.set(x, y, c, std::pow(v, gamma));
            }
        }
    }

    return newImage;
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
