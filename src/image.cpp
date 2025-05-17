#include <stdexcept>

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
    return newImage;
}

template<>
std::shared_ptr<Image<uint8_t>> readImage(const char * filename)
{
    int w = 0, h = 0;
    int numComponents = 0; // 0 = request actual number of components
    unsigned char * stbiData = stbi_load(filename, &w, &h, &numComponents, numComponents);

    if(!stbiData) {
        throw std::runtime_error(std::string("File not found: ") + filename);
    }

    const unsigned int numElements = w * h * numComponents;
    auto image = std::make_shared<Image<uint8_t>>(w, h, numComponents);

    for(unsigned int pi = 0; pi < numElements; pi++) {
        image->data[pi] = stbiData[pi];
    }
    stbi_image_free(stbiData);

    return image;
}

template<>
std::shared_ptr<Image<float>> readImage(const char * filename)
{
    int w = 0, h = 0;
    int numComponents = 0; // 0 = request actual number of components

    // Test patterns
    if(std::string(filename) == "grayramp") {
        auto image = std::make_shared<Image<float>>(256, 256, 3);
        *image = testpattern::grayRamp<float>(256, 256);
        return image;
    }

    if(stbi_is_hdr(filename)) {
        // Load float data with full dynamic range of the source image
        float * stbiData = stbi_loadf(filename, &w, &h, &numComponents, numComponents);

        if(!stbiData) {
            throw std::runtime_error(std::string("File not found: ") + filename);
        }

        const unsigned int numElements = w * h * numComponents;
        auto image = std::make_shared<Image<float>>(w, h, numComponents);

        for(unsigned int pi = 0; pi < numElements; pi++) {
            image->data[pi] = stbiData[pi];
        }

        stbi_image_free(stbiData);

        return image;
    }
    else {
        // Load float data, adjusting dynamic range if necessary
        unsigned char * stbiData = stbi_load(filename, &w, &h, &numComponents, numComponents);

        if(!stbiData) {
            throw std::runtime_error(std::string("File not found: ") + filename);
        }

        const unsigned int numElements = w * h * numComponents;
        auto image = std::make_shared<Image<float>>(w, h, numComponents);

        for(unsigned int pi = 0; pi < numElements; pi++) {
            image->data[pi] = float(stbiData[pi]) / 255.0f;
        }

        stbi_image_free(stbiData);

        return image;
    }
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

template<>
bool writeHDR<float>(const Image<float> & image, const char * filename)
{
    return !!stbi_write_hdr(filename, image.width, image.height,
                            image.numChannels, &image.data[0]);
}


template<>
Image<float> applyGamma(const Image<float> & image, float gamma)
{
    Image<float> newImage(image.width, image.height, image.numChannels);

    auto doGamma = [&](Image<float> & _, size_t x, size_t y, int c) {
        auto v = image.get(x, y, c);
        newImage.set(x, y, c, std::pow(v, gamma));
    };

    newImage.forEachPixelChannel(doGamma);

    return newImage;
}

template<>
Image<uint8_t> applyGamma(const Image<uint8_t> & image, float gamma)
{
    Image<uint8_t> newImage(image.width, image.height, image.numChannels);

    auto doGamma = [&](Image<uint8_t> & _, size_t x, size_t y, int c) {
        auto v = image.get(x, y, c);
        newImage.set(x, y, c, std::pow(float(v) / 255.0f, gamma) * 255.0f);
    };

    newImage.forEachPixelChannel(doGamma);

    return newImage;
}

template<typename T>
T Image<T>::maxValueAllChannels() const
{
    return *std::max_element(data.begin(), data.end());
}

template float Image<float>::maxValueAllChannels() const;

