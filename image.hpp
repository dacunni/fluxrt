#include <cassert>

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

