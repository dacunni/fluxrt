#include <iostream>
#include <vector>
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
    const int w = texture->width, h = texture->height;
    const int N = w * h;

    // Compute per-pixel sampling weights (cosine-elevation-weighted luminance)
    // and raw luminance sum for PDF normalization.
    // cosElevation corrects for the area distortion of the lat/lon projection:
    // pixels near the poles cover less solid angle and are down-weighted.
    std::vector<float> weights(N);
    float pdfSum     = 0.0f;
    float totalWeight = 0.0f;

    for(int y = 0; y < h; ++y) {
        float cosEl = std::cos(lerpFromTo<float>(float(y) + 0.5f, 0.0f, float(h),
                                                 -constants::PI_OVER_TWO, constants::PI_OVER_TWO));
        for(int x = 0; x < w; ++x) {
            float lum    = texture->channelSum(x, y);
            float weight = lum * cosEl * float(constants::TWO_PI);
            weights[y * w + x] = weight;
            pdfSum      += lum;
            totalWeight += weight;
        }
    }

    // PDF normalization factor — matches the convention used in the original code:
    //   pdf(x,y) = channelSum(x,y) / pdfSum / FOUR_PI * (w * h)
    pdfNormFactor = (pdfSum > 0.0f)
        ? float(N) / pdfSum / float(constants::FOUR_PI)
        : 0.0f;

    // Build Walker alias table from the cosine-weighted sampling distribution.
    // Construction runs once at load time; each sample costs 2 array lookups + 1 compare.
    aliasProb.assign(N, 1.0f);
    aliasIdx.assign(N, 0);

    // Scale weights so the average is 1.0
    const float invTotal = (totalWeight > 0.0f) ? float(N) / totalWeight : 0.0f;
    std::vector<float> scaled(N);
    for(int i = 0; i < N; ++i) {
        scaled[i] = weights[i] * invTotal;
    }

    // Vose's O(n) alias table construction
    std::vector<int> small, large;
    small.reserve(N);
    large.reserve(N);
    for(int i = 0; i < N; ++i) {
        if(scaled[i] < 1.0f) small.push_back(i);
        else                  large.push_back(i);
    }
    while(!small.empty() && !large.empty()) {
        int l = small.back(); small.pop_back();
        int g = large.back(); large.pop_back();
        aliasProb[l] = scaled[l];
        aliasIdx[l]  = uint32_t(g);
        scaled[g]    = (scaled[g] + scaled[l]) - 1.0f;
        if(scaled[g] < 1.0f) small.push_back(g);
        else                  large.push_back(g);
    }
    // Any remaining entries have probability 1 (already set by assign above)
}

vec2 LatLonEnvironmentMap::importanceSample(float e1, float e2, float & pdf) const
{
    const int N = int(aliasProb.size());
    const int i   = std::min(int(e1 * float(N)), N - 1);
    const int idx = (e2 < aliasProb[i]) ? i : int(aliasIdx[i]);

    const int x = idx % texture->width;
    const int y = idx / texture->width;

    pdf = texture->channelSum(x, y) * pdfNormFactor;
    return { float(x) + 0.5f, float(y) + 0.5f };
}

EnvironmentMapSample LatLonEnvironmentMap::importanceSampleFull(float e1, float e2) const
{
    const int N = int(aliasProb.size());
    const int i   = std::min(int(e1 * float(N)), N - 1);
    const int idx = (e2 < aliasProb[i]) ? i : int(aliasIdx[i]);

    const int x = idx % texture->width;
    const int y = idx / texture->width;

    const float pdf = texture->channelSum(x, y) * pdfNormFactor;

    // Convert pixel center to a spherical direction — same mapping as sampleRay's inverse
    const float PI     = float(constants::PI);
    const float TWO_PI = float(constants::TWO_PI);
    const float u     = (float(x) + 0.5f) / float(texture->width);
    const float v     = (float(y) + 0.5f) / float(texture->height);
    const float phi   = u * TWO_PI - PI;
    const float theta = v * PI;
    const float sinTheta = std::sin(theta);
    const Direction3 dir {
        std::sin(phi) * sinTheta,
        std::cos(theta),
        -std::cos(phi) * sinTheta
    };

    // Direct pixel fetch — no round-trip coordinate transform needed
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
    const int w = texture->width, h = texture->height;

    writePNG(*texture, "envmap_texture.png");

    // Reconstruct a PDF image for visualization
    Texture pdfTex(w, h, 1);
    for(int y = 0; y < h; ++y)
        for(int x = 0; x < w; ++x)
            pdfTex.set(x, y, 0, texture->channelSum(x, y) * pdfNormFactor);
    writePNG(applyGamma(pdfTex, 1.0/10.0), "envmap_pdf.png");
}
