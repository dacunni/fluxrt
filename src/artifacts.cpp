#include "artifacts.h"

Artifacts::Artifacts()
    : Artifacts(256, 256)
{
}

Artifacts::Artifacts(int w, int h)
    : w(w), h(h),
    hitMask(w, h, 1),
    isectDist(w, h, 3),
    isectNormal(w, h, 3),
    isectTangent(w, h, 3),
    isectBitangent(w, h, 3),
    isectTexCoord(w, h, 3),
    isectPos(w, h, 3),
    isectBasicLighting(w, h, 3),
    isectMatDiffuse(w, h, 3),
    isectMatSpecular(w, h, 3),
    isectAO(w, h, 3),
    isectTime(w, h, 3),
    pixelColor(w, h, 3),
    samplesPerPixel(w, h, 1)
{
    hitMask.setAll(0.0f);
    isectDist.setAll(0.0f);
    isectNormal.setAll(0.0f);
    isectTangent.setAll(0.0f);
    isectBitangent.setAll(0.0f);
    isectTexCoord.setAll(0.0f);
    isectPos.setAll(0.0f);
    isectBasicLighting.setAll(0.0f);
    isectMatDiffuse.setAll(0.0f);
    isectMatDiffuse.setAll(0.0f);
    isectAO.setAll(0.0f);
    isectTime.setAll(0.0f);
    pixelColor.setAll(0.0f);
    samplesPerPixel.setAll(0u);
}

void Artifacts::writeAll()
{
    writePNG(hitMask, prefix + "hit_mask.png");
    writePNG(isectDist, prefix + "isect_distance.png");
    writePNG(isectNormal, prefix + "isect_normal.png");
    writePNG(isectTangent, prefix + "isect_tangent.png");
    writePNG(isectBitangent, prefix + "isect_bitangent.png");
    writePNG(isectTexCoord, prefix + "isect_texcoord.png");
    writePNG(isectPos, prefix + "isect_position.png");
    writePNG(applyStandardGamma(isectBasicLighting), prefix + "isect_basic_lighting.png");
    writePNG(isectMatDiffuse, prefix + "isect_mat_diffuse.png");
    writePNG(isectMatSpecular, prefix + "isect_mat_specular.png");
    if(hasAO) {
        writePNG(applyStandardGamma(isectAO), prefix + "ao.png");
    }

    //writePNG(isectTime, prefix + "isect_time.png");

    auto scaledTime = isectTime;
    float maxTime = *std::max_element(begin(isectTime.data), end(isectTime.data));
    auto scaleToMax = [&](Image<float> & image, size_t x, size_t y, int c) {
        image.set(x, y, c, image.get(x, y, c) / maxTime);
    };
    scaledTime.forEachPixelChannel(scaleToMax);
    writePNG(scaledTime, prefix + "isect_time.png");

    writePixelColor();
}

void Artifacts::writePixelColor()
{
    auto finalPixelColor = pixelColor;
    auto divideByNumSamples = [&](Image<float> & image, size_t x, size_t y, int c) {
        auto numSamples = samplesPerPixel.get(x, y, 0);
        image.set(x, y, c, image.get(x, y, c) / float(numSamples));
    };
    finalPixelColor.forEachPixelChannel(divideByNumSamples);
    writePNG(applyStandardGamma(finalPixelColor), prefix + "color.png");
}

