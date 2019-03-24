#include <stdlib.h>
#include <iostream>
#include "micromath.h"

template<typename T>
void test_linear_interpolate(const std::string & label)
{
    int w = 256, h = 64;
    using ImageType = float;
    Image<ImageType> image(w, h, 1);
    ImageType imgMaxValue = color::channel::maxValue<ImageType>();

    image.setAll(0);

    T minValue = color::channel::minValue<T>();
    T maxValue = color::channel::maxValue<T>();

    for(int y = 0; y < h; y++) {
        for(int x = 0; x < w; x++) {
            float a = (float) x / (w - 1);
            T value = lerp(a, minValue, maxValue);
            image.set(x, y, 0, float(value) / maxValue * imgMaxValue);
        }
    }

    writePNG(applyStandardGamma(image), "lerp_" + label + ".png");
}

template<typename T>
void test_bilinear_interpolate(const std::string & label)
{
    int w = 256, h = 256;
    using ImageType = float;
    Image<ImageType> image(w, h, 1);
    ImageType imgMaxValue = color::channel::maxValue<ImageType>();

    image.setAll(0);

    T minValue = color::channel::minValue<T>();
    T maxValue = color::channel::maxValue<T>();

    T v11 = minValue; T v12 = maxValue / 4;
    T v21 = maxValue; T v22 = maxValue / 8;

    for(int y = 0; y < h; y++) {
        for(int x = 0; x < w; x++) {
            float a = (float) x / (w - 1);
            float b = (float) y / (h - 1);
            T value = bilerp(a, b, v11, v12, v21, v22);
            image.set(x, y, 0, float(value) / maxValue * imgMaxValue);
        }
    }

    writePNG(applyStandardGamma(image), "bilerp_" + label + ".png");
}

template<typename T>
void test_image_linear_interpolate(const std::string & label)
{
    int w = 256, h = 256;
    Image<T> imageSource(32, 32, 3);
    Image<T> imageInterp(w, h, 3);

    imageSource.setAll(0);
    imageInterp.setAll(0);

    T minValue = color::channel::minValue<T>();
    T maxValue = color::channel::maxValue<T>();

    imageSource.forEachPixel(
        [&](Image<T> & image, size_t x, size_t y) {
            float ay = (float) y / (image.height - 1);
            float ax = (float) x / (image.width - 1);
            
            if(((x / 4) & 0x1) ^ ((y / 4) & 0x1)) {
                image.set3(x, y, minValue, minValue, minValue);
            }
            else {
                image.set(x, y, 0, maxValue);
                image.set(x, y, 1, lerp(ax, minValue, maxValue));
                image.set(x, y, 2, lerp(ay, minValue, maxValue));
            }
        });

    float angle = 0.05 * M_PI;
    float rot[] = {
        std::cos(angle), std::sin(angle),
        -std::sin(angle), std::cos(angle)
    };

    imageInterp.forEachPixelChannel(
        [&](Image<T> & image, size_t x, size_t y, int ch) {
            float ay = (float) y / (h - 1) - 0.5f;
            float ax = (float) x / (w - 1) - 0.5f;
            float cx = rot[0] * ax + rot[1] * ay + 0.5f;
            float cy = rot[2] * ax + rot[3] * ay + 0.5f;
            imageInterp.set(x, y, ch, imageSource.lerp(cx * imageSource.width, cy * imageSource.height, ch));
        });

    writePNG(applyStandardGamma(imageSource), "lerp_image_" + label + "_source.png");
    writePNG(applyStandardGamma(imageInterp), "lerp_image_" + label + "_interp.png");
}

template<typename T>
void test_image_wrap_mode(typename Image<T>::OutOfBoundsBehavior behavior, const std::string & label)
{
    int w = 256, h = 256;
    int border = 64;
    Image<T> imageSource(w, h, 3);
    Image<T> imageInterp(w + border, h + border, 3);

    imageSource.outOfBoundsBehavior = behavior;
    imageSource.setAll(0);
    imageInterp.setAll(0);

    T minValue = color::channel::minValue<T>();
    T maxValue = color::channel::maxValue<T>();

    imageSource.forEachPixel(
        [&](Image<T> & image, size_t x, size_t y) {
            float ay = (float) y / (imageSource.height - 1);
            float ax = (float) x / (imageSource.width - 1);
            
            if(((x / 4) & 0x1) ^ ((y / 4) & 0x1)) {
                image.set3(x, y, minValue, minValue, minValue);
            }
            else {
                image.set(x, y, 0, lerp(ax, minValue, maxValue));
                image.set(x, y, 1, lerp(ay, minValue, maxValue));
                image.set(x, y, 2, 0);
            }
        });

    //float angle = 0.05 * M_PI;
    float angle = 0.0f;
    float rot[] = {
        std::cos(angle), std::sin(angle),
        -std::sin(angle), std::cos(angle)
    };

    imageInterp.forEachPixelChannel(
        [&](Image<T> & image, size_t x, size_t y, int ch) {
            float ay = (float) y / (h - 1) - 0.5f;
            float ax = (float) x / (w - 1) - 0.5f;
            float cx = rot[0] * ax + rot[1] * ay + 0.5f;
            float cy = rot[2] * ax + rot[3] * ay + 0.5f;
            imageInterp.set(x, y, ch, imageSource.lerp(cx * imageSource.width - border / 2, cy * imageSource.height - border / 2, ch));
        });

    writePNG(applyStandardGamma(imageInterp), "image_wrap_mode_" + label + "_source.png");
}

template<typename T>
void test_image_wrap_modes(const std::string & label)
{
    test_image_wrap_mode<T>(Image<T>::ClampOutOfBoundsCoordinate, std::string("clampcoord_") + label);
    test_image_wrap_mode<T>(Image<T>::ZeroValueOutOfBounds, std::string("zerovalue_") + label);
    test_image_wrap_mode<T>(Image<T>::Repeat, std::string("repeat_") + label);
}

int main(int argc, char ** argv)
{
    // Linear interpolation
    test_linear_interpolate<float>("float");
    test_linear_interpolate<double>("double");
    test_linear_interpolate<int8_t>("int8");
    test_linear_interpolate<int16_t>("int16");
    test_linear_interpolate<int32_t>("int32");
    test_linear_interpolate<int64_t>("int64");
    test_linear_interpolate<uint8_t>("uint8");
    test_linear_interpolate<uint16_t>("uint16");
    test_linear_interpolate<uint32_t>("uint32");
    test_linear_interpolate<uint64_t>("uint64");

    // Bilinear interpolation
    test_bilinear_interpolate<float>("float");
    test_bilinear_interpolate<double>("double");
    test_bilinear_interpolate<int8_t>("int8");
    test_bilinear_interpolate<int16_t>("int16");
    test_bilinear_interpolate<int32_t>("int32");
    test_bilinear_interpolate<int64_t>("int64");
    test_bilinear_interpolate<uint8_t>("uint8");
    test_bilinear_interpolate<uint16_t>("uint16");
    test_bilinear_interpolate<uint32_t>("uint32");
    test_bilinear_interpolate<uint64_t>("uint64");

    // Image linear interpolation
    test_image_linear_interpolate<float>("float");
    test_image_linear_interpolate<uint8_t>("uint8");

    // Out-of-bounds behavior
    test_image_wrap_modes<float>("float");
    test_image_wrap_modes<uint8_t>("uint8");

    return EXIT_SUCCESS;
}

