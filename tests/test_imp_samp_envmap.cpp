#include <stdlib.h>
#include <iostream>

#include "base.h"
#include "image.h"
#include "LatLonEnvironmentMap.h"
#include "rng.h"

int main(int argc, char ** argv)
{
#if 0
    LatLonEnvironmentMap latlonEnvmap;
    latlonEnvmap.loadFromFile("/Volumes/Extra/data/envmaps/hdrihaven/leadenhall_market_1k.hdr");
    //latlonEnvmap.loadFromFile("/Volumes/Extra/data/envmaps/hdrihaven/adams_place_bridge_1k.hdr");
    //latlonEnvmap.loadFromFile("/Volumes/Extra/data/envmaps/hdrihaven/street_lamp_1k.hdr");

#else
    int w = 512, h = 256;

    Image<float> latlonEnvmapData(w, h, 3);

    latlonEnvmapData.setAll(0.1);
    // Place some features in the environment map

    for(int y = h/4; y < h*3/4; y++) {
        for(int x = w/4; x < w*3/4; x++) {
            latlonEnvmapData.set(x, y, 1, 2.0);
        }
    }

    {
        float cx = 47.0f, cy = 56.2f, radius = 20.0f;

        for(int y = 0; y < h; y++) {
            float dy = float(y) - cy;
            for(int x = 0; x < w; x++) {
                float dx = float(x) - cx;
                if(dx * dx + dy * dy < radius * radius) {
                    latlonEnvmapData.set(x, y, 2, 10.0);
                }
            }
        }
    }

    {
        float cx = 300.0f, cy = 200.2f, radius = 50.0f;

        for(int y = 0; y < h; y++) {
            float dy = float(y) - cy;
            for(int x = 0; x < w; x++) {
                float dx = float(x) - cx;
                if(dx * dx + dy * dy < radius * radius) {
                    latlonEnvmapData.set3(x, y, 0.8, 5.8, 0.8);
                }
            }
        }
    }

    LatLonEnvironmentMap latlonEnvmap;
    latlonEnvmap.loadFromImage(latlonEnvmapData);
#endif

    Texture & texture = *latlonEnvmap.getTexture();
    writePNG(texture, "ll_envmap.png");

    //writePNG(*latlonEnvmap.rowSums, "ll_envmap_row_sums.png");

    Image<float> latlonEnvmapSamples(texture.width, texture.height, 1);

    RNG rng;
    int numSamples = 20000;
    float pdf;

#if 1
    // stratified sampling
    for(int sample = 0; sample < numSamples; ++sample) {
        vec2 e = rng.uniform2DRange01Stratified(200, 100, sample);
        vec2 coord = latlonEnvmap.importanceSample(e.x, e.y, pdf);
        latlonEnvmapSamples.set(coord.x, coord.y, 0, 1.0f);
    }
#else
    // random sampling
    for(int sample = 0; sample < numSamples; ++sample) {
        vec2 e = rng.uniform2DRange01();
        vec2 coord = latlonEnvmap.importanceSample(e.x, e.y, pdf);
        latlonEnvmapSamples.set(coord.x, coord.y, 0, 1.0f);
    }
#endif

    writePNG(latlonEnvmapSamples, "ll_envmap_samples.png");

    return EXIT_SUCCESS;
}

