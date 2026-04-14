#include <benchmark/benchmark.h>
#include <vector>
#include <cmath>

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
// Global fixture — built once, shared across all benchmarks
// ---------------------------------------------------------------------------

struct Fixture
{
    static constexpr int W = 1024;
    static constexpr int H = 512;

    Image<float>         img;
    LatLonEnvironmentMap latlon;

    Fixture()
        : img(makeSyntheticEnvMap(W, H))
    {
        latlon.loadFromImage(img);
    }
};

static Fixture * g_fixture = nullptr;

// ---------------------------------------------------------------------------
// BM_ImportanceSample: binary search lookup in isolation
// ---------------------------------------------------------------------------

static void BM_ImportanceSample(benchmark::State & state)
{
    LatLonEnvironmentMap & latlon = g_fixture->latlon;
    RNG rng;
    float pdf;
    for (auto _ : state) {
        vec2 e     = rng.uniform2DRange01();
        vec2 coord = latlon.importanceSample(e.x, e.y, pdf);
        benchmark::DoNotOptimize(coord);
        benchmark::DoNotOptimize(pdf);
    }
}
BENCHMARK(BM_ImportanceSample);

// ---------------------------------------------------------------------------
// BM_Full_OldPath: binary search + direction + sampleRay (atan2/acos/bilinear)
//   Approximates what importanceSampleDirection + sampleRay cost together
// ---------------------------------------------------------------------------

static void BM_Full_OldPath(benchmark::State & state)
{
    LatLonEnvironmentMap & latlon = g_fixture->latlon;
    RNG rng;
    for (auto _ : state) {
        vec2 e = rng.uniform2DRange01();

        // Same work as importanceSampleDirection
        RandomDirection d = latlon.importanceSampleDirection(e.x, e.y);

        // Then sampleRay: direction -> atan2/acos -> bilinear lookup
        Ray ray{ Position3(0.0f, 0.0f, 0.0f), d.direction };
        RadianceRGB rad = latlon.sampleRay(ray);

        benchmark::DoNotOptimize(d);
        benchmark::DoNotOptimize(rad);
    }
}
BENCHMARK(BM_Full_OldPath);

// ---------------------------------------------------------------------------
// BM_Full_NewPath: binary search + direction + direct pixel fetch
//   importanceSampleFull combines all three steps without the round-trip
// ---------------------------------------------------------------------------

static void BM_Full_NewPath(benchmark::State & state)
{
    LatLonEnvironmentMap & latlon = g_fixture->latlon;
    RNG rng;
    for (auto _ : state) {
        vec2 e              = rng.uniform2DRange01();
        EnvironmentMapSample s = latlon.importanceSampleFull(e.x, e.y);
        benchmark::DoNotOptimize(s);
    }
}
BENCHMARK(BM_Full_NewPath);

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
