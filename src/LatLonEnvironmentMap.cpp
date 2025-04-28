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

RadianceRGB LatLonEnvironmentMap::sampleRay(const Ray & ray)
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
        //float cosEl = std::cos(lerpFromTo<float>(float(y)+0.5f, 0.0f, float(h)+0.5f,
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

#if 0  // WORKING
    // Linear search y using row sums
    int y;
    for(y = 0; y < h - 1; ++y) {
        if(cumRows[y] > e2)
            break;
    }

    // Linear search for x in row given by y
    int x;
    for(x = 0; x < w - 1; ++x) {
        if(rowSums->get(x, y, 0) > e1)
            break;
    }

#else

    // Binary search for y using row sums

    int y1 = 0, y2 = h - 1;
    //int y1 = 0, y2 = h;
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
#endif

    pdf = pdf2D->get(x, y, 0) * w * h;

#if 0 // TODO: Seems okay, but needs more testing
    // Grab some lower order bits of the random numbers and treat them
    // as separate random numbers for subpixel sampling.
    float sx = float(x), sy = float(y);
    float shift = 2.0e4f;

    float e3 = e1 * shift - floor(e1 * shift);
    float e4 = e2 * shift - floor(e2 * shift);
    //printf("e1 %f e2 %f : e3 %f e4 %f\n", e1, e2, e3, e4); // TEMP

    sx += e3;
    sy += e4;

    return { sx, sy };
#else
    // No subpixel sampling
    return { float(x) + 0.5f, float(y) + 0.5f };
#endif
}

RandomDirection LatLonEnvironmentMap::importanceSampleDirection(float e1, float e2) const
{
    float pdf = 0.0f;
    vec2 pixel = importanceSample(e1, e2, pdf);
    TextureCoordinate texcoord = {
        pixel.x / float(texture->width),
        pixel.y / float(texture->height)
    };

    float PI = float(constants::PI);
    float TWO_PI = float(constants::TWO_PI);

    float phi = texcoord.u * TWO_PI - PI;
    float theta = texcoord.v * PI;

    Direction3 dir{
        std::sin(phi) * std::sin(theta),
        std::cos(theta),
        -std::cos(phi) * std::sin(theta)
    };

    return { dir, pdf };
}

void LatLonEnvironmentMap::saveDebugImages()
{
    writePNG(*texture, "envmap_texture.png");
    writePNG(*rowSums, "envmap_rowsums.png");
    writePNG(applyGamma(*pdf2D, 1.0/10.0), "envmap_pdf.png");
    //writePNG(*pdf2D, "envmap_pdf.png");
}

