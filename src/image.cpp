#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
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
std::shared_ptr<Image<uint8_t>> readImage(const char * filename)
{
    int w = 0, h = 0, numComponents = 3;
    unsigned char * stbiData = stbi_load(filename, &w, &h, &numComponents, 3);

    if(!stbiData) {
        throw std::runtime_error(std::string("File not found: ") + filename);
    }

    const unsigned int numElements = w * h * 3;
    auto image = std::make_shared<Image<uint8_t>>(w, h, 3);

    for(unsigned int pi = 0; pi < numElements; pi++) {
        image->data[pi] = stbiData[pi];
    }
    stbi_image_free(stbiData);

    return image;
}

template<>
std::shared_ptr<Image<float>> readImage(const char * filename)
{
    int w = 0, h = 0, numComponents = 3;
    unsigned char * stbiData = stbi_load(filename, &w, &h, &numComponents, 3);

    if(!stbiData) {
        throw std::runtime_error(std::string("File not found: ") + filename);
    }

    const unsigned int numElements = w * h * 3;
    auto image = std::make_shared<Image<float>>(w, h, 3);

    for(unsigned int pi = 0; pi < numElements; pi++) {
        image->data[pi] = float(stbiData[pi]) / 255.0f;
    }
    stbi_image_free(stbiData);

    return std::move(image);
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


template<>
Image<float> applyGamma(const Image<float> & image, float gamma)
{
    Image<float> newImage(image.width, image.height, image.numChannels);

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

template<>
Image<uint8_t> applyGamma(const Image<uint8_t> & image, float gamma)
{
    Image<uint8_t> newImage(image.width, image.height, image.numChannels);

    for(int y = 0; y < image.height; ++y) {
        for(int x = 0; x < image.width; ++x) {
            for(int c = 0; c < image.numChannels; ++c) {
                auto v = image.get(x, y, c);
                newImage.set(x, y, c, std::pow(float(v) / 255.0f, gamma) * 255.0f);
            }
        }
    }

    return newImage;
}

