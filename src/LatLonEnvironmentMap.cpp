#include <iostream>
#include "constants.h"
#include "LatLonEnvironmentMap.h"
#include "Ray.h"
#include "vectortypes.h"
#include "interpolation.h"
#include "rng.h"

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

    // Debug
    //saveDebugImages();
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
    //debugSamples = std::make_shared<Texture>(w, h, 1);

    float totalCumVal = 0.0f;
    
    for(int y = 0; y < h; ++y) {
        // Weight by the cosine of the elevation angle to account for
        // area distortion in a flattened lat/lon representation
        float cosEl = std::cos(lerpFromTo<float>(y, 0, h, -constants::PI_OVER_TWO, constants::PI_OVER_TWO));
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
    for(int y = 0; y < h; ++y) {
        for(int x = 0; x < w; ++x) {
            pdf2D->set(x, y, 0, pdf2D->get(x, y, 0) / totalCumVal);
        }
    }
}

vec2 LatLonEnvironmentMap::importanceSample(float e1, float e2, float & pdf) const
{
    const int w = texture->width, h = texture->height;

#if 0
    // Linear search y using row sums
    int y;
    for(y = 0; y < h - 1; ++y) {
        if(cumRows[y+1] > e2)
            break;
    }

    // Linear search for x in row given by y
    int x;
    for(x = 0; x < w - 1; ++x) {
        if(rowSums->get(x+1, y, 0) > e1)
            break;
    }
#else
    // Binary search for y using row sums

    int y1 = 0, y2 = h - 1;
    int y = (y1 + y2) / 2;

    while(y1 < y2 - 1) {
        float value = cumRows[y];
        if(e2 < value) { y2 = y; }
        else           { y1 = y; }
        y = (y1 + y2) / 2;
    }

    // Binary search for x in row given by y

    int x1 = 0, x2 = w - 1;
    int x = (x1 + x2) / 2;

    while(x1 < x2 - 1) {
        float value = rowSums->get(x, y, 0);
        if(e1 <= value) { x2 = x; }
        else            { x1 = x; }
        x = (x1 + x2) / 2;
    }
#endif

    pdf = pdf2D->get(x, y, 0) * w * h;

    // Debug
    //debugSamples->set(x, y, 0, 1.0f);

    // FIXME - choose subpixel coordinate
    return { float(x) + 0.5f, float(y) + 0.5f };
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
}

void LatLonEnvironmentMap::saveDebugSampleImage()
{
    if(debugSamples) {
        writePNG(*debugSamples, "envmap_samples.png");
    }
}

