#include <benchmark/benchmark.h>
#include "rng.h"

static void RandomUniform01(benchmark::State& state) {
    RNG rng;
    for (auto _ : state) {
        benchmark::DoNotOptimize(rng.uniform01());
    }
}
BENCHMARK(RandomUniform01);

static void RandomUniformRange(benchmark::State& state) {
    RNG rng;
    for (auto _ : state) {
        benchmark::DoNotOptimize(rng.uniformRange(2.4, 7.3));
    }
}
BENCHMARK(RandomUniformRange);

static void RandomUniformUnitCirlce(benchmark::State& state) {
    RNG rng;
    float x, y;
    for (auto _ : state) {
        rng.uniformUnitCircle(x, y);
        benchmark::DoNotOptimize(x);
        benchmark::DoNotOptimize(y);
    }
}
BENCHMARK(RandomUniformUnitCirlce);

static void RandomUniformCirlce(benchmark::State& state) {
    RNG rng;
    float x, y;
    for (auto _ : state) {
        rng.uniformCircle(3.0f, x, y);
        benchmark::DoNotOptimize(x);
        benchmark::DoNotOptimize(y);
    }
}
BENCHMARK(RandomUniformCirlce);

static void RandomSurfaceUnitSphereXYZ(benchmark::State& state) {
    RNG rng;
    float x, y, z;
    for (auto _ : state) {
        rng.uniformSurfaceUnitSphere(x, y, z);
        benchmark::DoNotOptimize(x);
        benchmark::DoNotOptimize(y);
        benchmark::DoNotOptimize(z);
    }
}
BENCHMARK(RandomSurfaceUnitSphereXYZ);

static void RandomSurfaceUnitHalfSphereXYZ(benchmark::State& state) {
    RNG rng;
    Direction3 d, n(0, 1, 0);
    for (auto _ : state) {
        rng.uniformSurfaceUnitHalfSphere(n, d);
        benchmark::DoNotOptimize(n);
        benchmark::DoNotOptimize(d);
    }
}
BENCHMARK(RandomSurfaceUnitHalfSphereXYZ);

static void RandomSurfaceUnitHalfSphereCosineDistributionXYZ(benchmark::State& state) {
    RNG rng;
    Direction3 d, n(0, 1, 0);
    for (auto _ : state) {
        rng.cosineAboutDirection(n, d);
        benchmark::DoNotOptimize(n);
        benchmark::DoNotOptimize(d);
    }
}
BENCHMARK(RandomSurfaceUnitHalfSphereCosineDistributionXYZ);

BENCHMARK_MAIN();

