#include "artifacts.h"
#include "timer.h"
#include "tonemapping.h"

Artifacts::Artifacts()
    : Artifacts(256, 256)
{
}

Artifacts::Artifacts(int w, int h)
    : 
    hitMask(w, h, 1),
    isectDist(w, h, 3),
    isectNormal(w, h, 3),
    isectTangent(w, h, 3),
    isectBitangent(w, h, 3),
    isectTexCoord(w, h, 3),
    //isectPos(w, h, 3),
    isectBasicLighting(w, h, 3),
    isectMatDiffuse(w, h, 3),
    isectMatSpecular(w, h, 3),
    isectAO(w, h, 3),
    isectTime(w, h, 3),
    pixelColor(w, h, 3),
    samplesPerPixel(w, h, 1),
    runningVarianceM(w, h, 3),
    runningVarianceS(w, h, 3),
    w(w), h(h)
{
    hitMask.setAll(0.0f);
    isectDist.setAll(0.0f);
    isectNormal.setAll(0.0f);
    isectTangent.setAll(0.0f);
    isectBitangent.setAll(0.0f);
    isectTexCoord.setAll(0.0f);
    //isectPos.setAll(0.0f);
    isectBasicLighting.setAll(0.0f);
    isectMatDiffuse.setAll(0.0f);
    isectMatDiffuse.setAll(0.0f);
    isectAO.setAll(0.0f);
    isectTime.setAll(0.0f);
    pixelColor.setAll(0.0f);
    runningVarianceS.setAll(0.0f);
    runningVarianceM.setAll(0.0f);
    samplesPerPixel.setAll(0u);
}

void Artifacts::writeAll()
{
    printf("Flushing artifacts\n");
    auto artifactWriteTimer = WallClockTimer::makeRunningTimer();
    writePNG(hitMask, prefix + "hit_mask.png");
    writePNG(isectDist, prefix + "isect_distance.png");
    writePNG(isectNormal, prefix + "isect_normal.png");
    writePNG(isectTangent, prefix + "isect_tangent.png");
    writePNG(isectBitangent, prefix + "isect_bitangent.png");
    writePNG(isectTexCoord, prefix + "isect_texcoord.png");
    //writePNG(isectPos, prefix + "isect_position.png");
    if(doBasicLighting) {
        writePNG(applyStandardGamma(isectBasicLighting), prefix + "isect_basic_lighting.png");
    }
    writePNG(isectMatDiffuse, prefix + "isect_mat_diffuse.png");
    writePNG(isectMatSpecular, prefix + "isect_mat_specular.png");
    if(hasAO) {
        writePNG(applyStandardGamma(isectAO), prefix + "ao.png");
    }

#if 0
    auto scaledTime = isectTime;
    float maxTime = *std::max_element(begin(isectTime.data), end(isectTime.data));
    auto scaleToMax = [&](Image<float> & image, size_t x, size_t y, int c) {
        image.set(x, y, c, image.get(x, y, c) / maxTime);
    };
    scaledTime.forEachPixelChannel(scaleToMax);
#endif
    //writePNG(scaledTime, prefix + "isect_time.png");
    //writeHDR(scaledTime, prefix + "isect_time.hdr");
    writeHDR(isectTime, prefix + "isect_time.hdr");

    auto stddev = runningVarianceS;
    auto makeStdDev = [&](Image<float> & image, size_t x, size_t y, int c) {
        auto Np = samplesPerPixel.get(x, y, 0);
        if(Np > 1) {
            float var = image.get(x, y, c) / float(Np - 1);
            image.set(x, y, c, std::sqrt(var));
        }
    };
    stddev.forEachPixelChannel(makeStdDev);
    writePNG(stddev, prefix + "isect_stddev.png");

    writePixelColor();
    auto artifactWriteTime = artifactWriteTimer.elapsed();
    printf("Artifacts written in %f sec\n", artifactWriteTime);
}

void Artifacts::writePixelColor()
{
    auto finalPixelColor = pixelColor;
    auto divideByNumSamples = [&](Image<float> & image, size_t x, size_t y, int c) {
        auto numSamples = samplesPerPixel.get(x, y, 0);
        float value = image.get(x, y, c) / float(numSamples);
        image.set(x, y, c, value);
    };
    finalPixelColor.forEachPixelChannel(divideByNumSamples);
    writePNG(applyStandardGamma(finalPixelColor), prefix + "color.png");
    writeHDR(finalPixelColor, prefix + "color.hdr");

    //float maxValue = finalPixelColor.maxValueAllChannels();
    float maxValue = 4.0f;
    auto applyToneMap = [&](Image<float> & image, size_t x, size_t y, int c) {
        float value = image.get(x, y, c);
        //value = tonemapping::reinhard(value);
        value = tonemapping::reinhardExtended(value, maxValue);
        image.set(x, y, c, value);
    };
    finalPixelColor.forEachPixelChannel(applyToneMap);
    writePNG(applyStandardGamma(finalPixelColor), prefix + "color_tone_mapped.png");
}

