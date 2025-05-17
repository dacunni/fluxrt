#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <vector>
#include <string>
#include <memory>
#include <functional>

#include "base.h"
#include "color.h"

template<typename T>
struct Image
{
    Image(size_t w, size_t h, int channels);
    ~Image() = default;

    inline T get(size_t x, size_t y, int channel) const;
    inline ColorRGB getRGB(size_t x, size_t y) const;
    inline T channelSum(size_t x, size_t y) const;
    inline T lerp(float x, float y, int channel) const;
    inline T lerpUV(float u, float v, int channel) const;
    inline ColorRGB lerpUV3(float u, float v) const;

    inline void accum(size_t x, size_t y, int channel, T value);
    inline void accum(size_t x, size_t y, const ColorRGB & c);
    inline void accum3(size_t x, size_t y, T v0, T v1, T v2);

    inline void set(size_t x, size_t y, int channel, T value);
    inline void set3(size_t x, size_t y, T v0, T v1, T v2);
    inline void set3(size_t x, size_t y, const ColorRGB & c);
    inline void setAll(T value);

    T maxValueAllChannels() const;

    inline size_t index(size_t x, size_t y, int channel) const;

    using PixelFunction = std::function<void(Image<T> &,
                                             size_t /*x*/,
                                             size_t /*y*/)>;
    using PixelChannelFunction = std::function<void(Image<T> &,
                                                    size_t /*x*/,
                                                    size_t /*y*/,
                                                    int    /*c*/)>;
    void forEachPixel(const PixelFunction & fn);
    void forEachPixelChannel(const PixelChannelFunction & fn);

    void forEachPixelInRow(size_t y, const PixelFunction & fn);
    void forEachPixelInColumn(size_t x, const PixelFunction & fn);

    enum OutOfBoundsBehavior {
        ClampOutOfBoundsCoordinate,
        ZeroValueOutOfBounds,
        Repeat
    };

    std::vector<T> data;
    size_t width;
    size_t height;
    int numChannels;
    OutOfBoundsBehavior outOfBoundsBehavior = ZeroValueOutOfBounds;
};

template<typename T>
std::shared_ptr<Image<T>> readImage(const char * filename);
template<typename T>
std::shared_ptr<Image<T>> readImage(const std::string & filename) { return readImage<T>(filename.c_str()); }

template<typename T>
bool writePNG(const Image<T> & image, const char * filename);
template<typename T>
bool writePNG(const Image<T> & image, const std::string & filename) { return writePNG(image, filename.c_str()); }

template<typename T>
bool writeHDR(const Image<T> & image, const char * filename);
template<typename T>
bool writeHDR(const Image<T> & image, const std::string & filename) { return writeHDR(image, filename.c_str()); }

template<typename T, typename S>
Image<T> convert(const Image<S> & image);

template<typename T>
Image<T> applyGamma(const Image<T> & image, float gamma);

static const float gammaCorrectionFactor = 1.0f / 2.4f;

template<typename T>
Image<T> applyStandardGamma(const Image<T> & image) {
    return applyGamma(image, gammaCorrectionFactor);
}

template<typename T>
Image<T> applyInverseStandardGamma(const Image<T> & image) {
    return applyGamma(image, 1.0f / gammaCorrectionFactor);
}

namespace testpattern {

template<typename T>
Image<T> grayRamp(size_t xsize, size_t ysize);

template<typename T>
Image<T> colorRange();

template<typename T>
Image<T> checkerBoardBlackAndWhite(size_t w, size_t h,
                                   size_t checkSize, int numChannels); 

}; // namespace testpattern


#include "image.hpp"
#endif
