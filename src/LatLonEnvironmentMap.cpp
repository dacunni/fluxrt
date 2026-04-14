#include <iostream>
#include "constants.h"
#include "LatLonEnvironmentMap.h"
#include "Ray.h"
#include "vectortypes.h"
#include "interpolation.h"
#include "rng.h"
#include "constants.h"

void LatLonEnvironmentMap::loadFromFile(const std::string & filename)
{
    auto rawTexture = readImage<float>(filename);
    if(!rawTexture) {
        throw 0;
    }

    // FIXME: This is making excessive copies
    texture = std::make_shared<Texture>(rawTexture->width, rawTexture->height, rawTexture->numChannels);
    *texture = *rawTexture;

    texture->outOfBoundsBehavior = Texture::Repeat;

    buildImportanceSampleLookup();
}

void LatLonEnvironmentMap::loadFromImage(const Image<float> & image)
{
    // FIXME: This is making excessive copies
    texture = std::make_shared<Texture>(image.width, image.height, image.numChannels);
    *texture = image;

    texture->outOfBoundsBehavior = Texture::Repeat;

    buildImportanceSampleLookup();
}

RadianceRGB LatLonEnvironmentMap::sampleRay(const Ray & ray) const
{
    auto & D = ray.direction;

    TextureCoordinate texcoord;

    float PI = float(constants::PI);

    // FIXME: should this be atan2(-D.z, D.x) ?
    texcoord.u = 0.5f * (1.0f + std::atan2(D.x, -D.z) / PI);
    texcoord.v = std::acos(D.y) / PI;

    ColorRGB sample = texture->lerpUV3(texcoord.u, texcoord.v);

    return { scaleFactor * sample.r,
             scaleFactor * sample.g,
             scaleFactor * sample.b };
}

void LatLonEnvironmentMap::buildImportanceSampleLookup()
{
    int w = texture->width, h = texture->height;

    rowSums = std::make_shared<Texture>(w, h, 1);
    cumRows.resize(texture->height);
    pdf2D = std::make_shared<Texture>(w, h, 1);

    float totalCumVal = 0.0f;

    for(int y = 0; y < h; ++y) {
        // Weight by the cosine of the elevation angle to account for
        // area distortion in a flattened lat/lon representation
        float cosEl = std::cos(lerpFromTo<float>(float(y)+0.5f, 0.0f, float(h),
                                                 -constants::PI_OVER_TWO, constants::PI_OVER_TWO));
        // Compute cumulative sums for each row
        float rowCumVal = 0.0f;
        for(int x = 0; x < w; ++x) {
            float value = texture->channelSum(x, y);
            rowCumVal += value * constants::TWO_PI;
            pdf2D->set(x, y, 0, value);
            rowSums->set(x, y, 0, rowCumVal);
        }
        totalCumVal += rowCumVal * cosEl;
        cumRows[y] = totalCumVal;

        // Normalize PDFs and CDFs to [0, 1]
        if(rowCumVal > 0.0f) {
            for(int x = 0; x < w; ++x) {
                rowSums->set(x, y, 0, rowSums->get(x, y, 0) / rowCumVal);
            }
        }
    }
    // Normalize cumulative row sums to [0, 1]
    if(totalCumVal > 0.0f) {
        for(int y = 0; y < h; ++y) {
            cumRows[y] /= totalCumVal;
        }
    }

    float pdfSum = 0.0f;
    pdf2D->forEachPixel([&pdfSum](Image<float> & img, size_t x, size_t y) {
                    pdfSum += img.get(x, y, 0); });

    // Normalize the PDF
    for(int y = 0; y < h; ++y) {
        for(int x = 0; x < w; ++x) {
            pdf2D->set(x, y, 0, pdf2D->get(x, y, 0) / pdfSum / constants::FOUR_PI);
        }
    }
}

vec2 LatLonEnvironmentMap::importanceSample(float e1, float e2, float & pdf) const
{
    const int w = texture->width, h = texture->height;

    // Binary search for y using row sums
    int y1 = 0, y2 = h - 1;
    int y = (y1 + y2) / 2;

    while(y1 < y2) {
        float value = cumRows[y];
        if(value > e2) { y2 = y; }
        else           { y1 = y + 1; }
        y = (y1 + y2) / 2;
    }

    // Binary search for x in row given by y
    int x1 = 0, x2 = w - 1;
    int x = (x1 + x2) / 2;

    while(x1 < x2) {
        float value = rowSums->get(x, y, 0);
        if(value > e1) { x2 = x; }
        else           { x1 = x + 1; }
        x = (x1 + x2) / 2;
    }

    pdf = pdf2D->get(x, y, 0) * w * h;
    return { float(x) + 0.5f, float(y) + 0.5f };
}

EnvironmentMapSample LatLonEnvironmentMap::importanceSampleFull(float e1, float e2) const
{
    float pdf;
    vec2 pixel = importanceSample(e1, e2, pdf);

    const int x = int(pixel.x);
    const int y = int(pixel.y);

    // Convert pixel center to spherical direction
    const float PI     = float(constants::PI);
    const float TWO_PI = float(constants::TWO_PI);
    const float u      = pixel.x / float(texture->width);
    const float v      = pixel.y / float(texture->height);
    const float phi    = u * TWO_PI - PI;
    const float theta  = v * PI;
    const float sinTheta = std::sin(theta);
    const Direction3 dir {
        std::sin(phi) * sinTheta,
        std::cos(theta),
        -std::cos(phi) * sinTheta
    };

    // Direct pixel fetch — skips the atan2/acos/bilinear round-trip of sampleRay
    const RadianceRGB rad {
        scaleFactor * texture->get(x, y, 0),
        scaleFactor * texture->get(x, y, 1),
        scaleFactor * texture->get(x, y, 2)
    };

    return { dir, pdf, rad };
}

RandomDirection LatLonEnvironmentMap::importanceSampleDirection(float e1, float e2) const
{
    auto s = importanceSampleFull(e1, e2);
    return { s.direction, s.pdf };
}

void LatLonEnvironmentMap::saveDebugImages()
{
    writePNG(*texture, "envmap_texture.png");
    writePNG(*rowSums, "envmap_rowsums.png");
    writePNG(applyGamma(*pdf2D, 1.0/10.0), "envmap_pdf.png");
}
