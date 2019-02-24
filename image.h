#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <vector>

template<typename T>
struct Image
{
    Image(size_t w, size_t h, int channels);
    ~Image() = default;

    inline T get(size_t x, size_t y, int channel) const;

    inline void set(size_t x, size_t y, int channel, T value);
    inline void set3(size_t x, size_t y, T v0, T v1, T v2);
    inline void setAll(T value);

    inline size_t index(size_t x, size_t y, int channel) const;

    std::vector<T> data;
    size_t width;
    size_t height;
    int numChannels;
};

template<typename T>
bool writePNG(const Image<T> & image, const char * filename);
//template<typename T>
//bool writeHDR(const Image<T> & image, const char * filename);

template<typename T, typename S>
Image<T> convert(const Image<S> & image);

#include "image.hpp"
#endif
