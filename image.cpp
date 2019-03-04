#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "base.h"
#include "image.h"

template<>
Image<uint8_t> convert<uint8_t, float>(const Image<float> & image)
{
    Image<uint8_t> newImage(image.width, image.height, image.numChannels);
    std::transform(image.data.begin(), image.data.end(),
                   newImage.data.begin(),
                   [](float v) { return clamp01(v) * 255.0f; });
    return std::move(newImage);
}

template<>
bool writePNG<uint8_t>(const Image<uint8_t> & image, const char * filename)
{
    return !!stbi_write_png(filename, image.width, image.height,
                            image.numChannels, &image.data[0],
                            image.width * image.numChannels);
}

template<>
bool writePNG<float>(const Image<float> & image, const char * filename)
{
    auto imageU8 = convert<uint8_t, float>(image);
    return writePNG(imageU8, filename);
}

#if 0
template<>
bool writeHDR<float>(const Image<float> & image, const char * filename)
{
    return !!stbi_write_hdr(filename, image.width, image.height,
                            image.numChannels, &image.data[0]);
}
#endif


