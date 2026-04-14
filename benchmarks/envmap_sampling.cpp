#include <benchmark/benchmark.h>
#include <vector>
#include <cmath>
#include <algorithm>

#include "image.h"
#include "LatLonEnvironmentMap.h"
#include "rng.h"
#include "interpolation.h"
#include "constants.h"
#include "vectortypes.h"
#include "Ray.h"

// ---------------------------------------------------------------------------
// Synthetic environment map: uniform base + two bright circular features
// ---------------------------------------------------------------------------

static Image<float> makeSyntheticEnvMap(int w, int h)
{
    Image<float> img(w, h, 3);
    img.setAll(0.1f);

    auto circle = [&](float cx, float cy, float r, float r_val, float g_val, float b_val) {
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x) {
                float dx = float(x) - cx, dy = float(y) - cy;
                if (dx*dx + dy*dy < r*r)
                    img.set3(x, y, r_val, g_val, b_val);
            }
    };

    circle(w * 0.20f, h * 0.25f, h * 0.12f, 10.0f,  8.0f, 5.0f);
    circle(w * 0.65f, h * 0.40f, h * 0.08f,  4.0f,  6.0f, 4.0f);

    return img;
}

// ---------------------------------------------------------------------------
// "Old" binary-search sampler — reimplements the pre-alias-table CDF logic
// using flat std::vectors (slightly faster than the original Texture-based
// approach, so this is generous to the old code)
// ---------------------------------------------------------------------------

struct BinarySearchSampler
{
    int w = 0, h = 0;
    std::vector<float> cumRows;  // h elements: normalized row CDF
    std::vector<float> rowSums;  // w*h elements: per-row normalized CDFs
    std::vector<float> pixelPdf; // w*h elements: precomputed PDF

    void build(const Image<float> & img)
    {
        w = int(img.width);
        h = int(img.height);
        cumRows.resize(h);
        rowSums.resize(w * h);
        pixelPdf.resize(w * h);

        float totalCumVal = 0.0f;
        for (int y = 0; y < h; ++y) {
            float cosEl = std::cos(lerpFromTo<float>(float(y) + 0.5f, 0.0f, float(h),
                                                     -float(constants::PI_OVER_TWO),
                                                      float(constants::PI_OVER_TWO)));
            float rowCumVal = 0.0f;
            for (int x = 0; x < w; ++x) {
                float value    = img.channelSum(x, y);
                rowCumVal     += value * float(constants::TWO_PI);
                pixelPdf[y*w + x] = value;
                rowSums [y*w + x] = rowCumVal;
            }
            totalCumVal += rowCumVal * cosEl;
            cumRows[y]   = totalCumVal;

            if (rowCumVal > 0.0f)
                for (int x = 0; x < w; ++x)
                    rowSums[y*w + x] /= rowCumVal;
        }
        if (totalCumVal > 0.0f)
            for (int y = 0; y < h; ++y)
                cumRows[y] /= totalCumVal;

        float pdfSum = 0.0f;
        for (float v : pixelPdf) pdfSum += v;
        float invPdfNorm = (pdfSum > 0.0f) ? 1.0f / (pdfSum * float(constants::FOUR_PI)) : 0.0f;
        for (float & v : pixelPdf) v *= invPdfNorm;
    }

    // Returns the sampled PDF; mirrors the original importanceSample() return value
    float sample(float e1, float e2) const
    {
        // Binary search for row
        int y1 = 0, y2 = h - 1, y = (y1 + y2) / 2;
        while (y1 < y2) {
            if (cumRows[y] > e2) y2 = y;
            else                  y1 = y + 1;
            y = (y1 + y2) / 2;
        }

        // Binary search for column in selected row
        int x1 = 0, x2 = w - 1, x = (x1 + x2) / 2;
        while (x1 < x2) {
            if (rowSums[y*w + x] > e1) x2 = x;
            else                         x1 = x + 1;
            x = (x1 + x2) / 2;
        }

        return pixelPdf[y*w + x] * float(w * h);
    }
};

// ---------------------------------------------------------------------------
// Global fixture — built once, shared across all benchmarks
// ---------------------------------------------------------------------------

struct Fixture
{
    static constexpr int W = 1024;
    static constexpr int H = 512;

    Image<float>          img;
    BinarySearchSampler   bss;
    LatLonEnvironmentMap  latlon;

    Fixture()
        : img(makeSyntheticEnvMap(W, H))
    {
        bss.build(img);
        latlon.loadFromImage(img);
    }
};

static Fixture * g_fixture = nullptr;

// ---------------------------------------------------------------------------
// BM1: importanceSample only — binary search vs alias table
//      Tests the core CDF/alias lookup in isolation (no direction, no radiance)
// ---------------------------------------------------------------------------

static void BM_BinarySearch_ImportanceSample(benchmark::State & state)
{
    const BinarySearchSampler & bss = g_fixture->bss;
    RNG rng;
    for (auto _ : state) {
        vec2 e   = rng.uniform2DRange01();
        float pdf = bss.sample(e.x, e.y);
        benchmark::DoNotOptimize(pdf);
    }
}
BENCHMARK(BM_BinarySearch_ImportanceSample);

static void BM_AliasTable_ImportanceSample(benchmark::State & state)
{
    LatLonEnvironmentMap & latlon = g_fixture->latlon;
    RNG rng;
    float pdf;
    for (auto _ : state) {
        vec2 e    = rng.uniform2DRange01();
        vec2 coord = latlon.importanceSample(e.x, e.y, pdf);
        benchmark::DoNotOptimize(coord);
        benchmark::DoNotOptimize(pdf);
    }
}
BENCHMARK(BM_AliasTable_ImportanceSample);

// ---------------------------------------------------------------------------
// BM2: full sampling path — direction + radiance
//      "Old" = binary search + pixel→trig→direction + sampleRay(atan2/acos/bilinear)
//      "New" = alias table + importanceSampleFull (direct pixel fetch)
// ---------------------------------------------------------------------------

static void BM_BinarySearch_Full(benchmark::State & state)
{
    const BinarySearchSampler & bss    = g_fixture->bss;
    LatLonEnvironmentMap &      latlon = g_fixture->latlon;
    const float PI     = float(constants::PI);
    const float TWO_PI = float(constants::TWO_PI);
    const int   W = bss.w, H = bss.h;
    RNG rng;

    for (auto _ : state) {
        vec2 e = rng.uniform2DRange01();

        // Binary search for (x, y)
        int y1 = 0, y2 = H-1, y = (y1+y2)/2;
        while (y1 < y2) {
            if (bss.cumRows[y] > e.y) y2 = y; else y1 = y+1;
            y = (y1+y2)/2;
        }
        int x1 = 0, x2 = W-1, x = (x1+x2)/2;
        while (x1 < x2) {
            if (bss.rowSums[y*W + x] > e.x) x2 = x; else x1 = x+1;
            x = (x1+x2)/2;
        }
        float pdf = bss.pixelPdf[y*W + x] * float(W*H);

        // Old direction path: pixel center → spherical → Cartesian
        float u     = (float(x) + 0.5f) / float(W);
        float v     = (float(y) + 0.5f) / float(H);
        float phi   = u * TWO_PI - PI;
        float theta = v * PI;
        float sinT  = std::sin(theta);
        Direction3 dir { std::sin(phi)*sinT, std::cos(theta), -std::cos(phi)*sinT };

        // Old radiance path: direction → atan2/acos → bilinear texture lookup
        Ray ray{ Position3(0.0f, 0.0f, 0.0f), dir };
        RadianceRGB rad = latlon.sampleRay(ray);

        benchmark::DoNotOptimize(pdf);
        benchmark::DoNotOptimize(dir);
        benchmark::DoNotOptimize(rad);
    }
}
BENCHMARK(BM_BinarySearch_Full);

static void BM_AliasTable_Full(benchmark::State & state)
{
    LatLonEnvironmentMap & latlon = g_fixture->latlon;
    RNG rng;
    for (auto _ : state) {
        vec2 e              = rng.uniform2DRange01();
        EnvironmentMapSample s = latlon.importanceSampleFull(e.x, e.y);
        benchmark::DoNotOptimize(s);
    }
}
BENCHMARK(BM_AliasTable_Full);

// ---------------------------------------------------------------------------

int main(int argc, char ** argv)
{
    Fixture fixture;
    g_fixture = &fixture;

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
    return 0;
}
