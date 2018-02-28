#include <benchmark/benchmark.h>
#include "FastRender.h"

static void RandomUniform01(benchmark::State& state) {
    RandomNumberGenerator rng;
    for (auto _ : state) {
        benchmark::DoNotOptimize(rng.uniform01());
    }
}
BENCHMARK(RandomUniform01);

static void RandomUniformRange(benchmark::State& state) {
    RandomNumberGenerator rng;
    for (auto _ : state) {
        benchmark::DoNotOptimize(rng.uniformRange(2.4, 7.3));
    }
}
BENCHMARK(RandomUniformRange);

static void RandomUniformUnitCirlce(benchmark::State& state) {
    RandomNumberGenerator rng;
    float x, y;
    for (auto _ : state) {
        rng.uniformUnitCircle(x, y);
        benchmark::DoNotOptimize(x);
        benchmark::DoNotOptimize(y);
    }
}
BENCHMARK(RandomUniformUnitCirlce);

static void RandomUniformCirlce(benchmark::State& state) {
    RandomNumberGenerator rng;
    float x, y;
    for (auto _ : state) {
        rng.uniformCircle(3.0f, x, y);
        benchmark::DoNotOptimize(x);
        benchmark::DoNotOptimize(y);
    }
}
BENCHMARK(RandomUniformCirlce);

static void RandomSurfaceUnitSphereXYZ(benchmark::State& state) {
    RandomNumberGenerator rng;
    float x, y, z;
    for (auto _ : state) {
        rng.uniformSurfaceUnitSphere(x, y, z);
        benchmark::DoNotOptimize(x);
        benchmark::DoNotOptimize(y);
        benchmark::DoNotOptimize(z);
    }
}
BENCHMARK(RandomSurfaceUnitSphereXYZ);

static void RandomSurfaceUnitSphereV4(benchmark::State& state) {
    RandomNumberGenerator rng;
    Vector4 v;
    for (auto _ : state) {
        rng.uniformSurfaceUnitSphere(v);
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(RandomSurfaceUnitSphereV4);

static void RandomSurfaceUnitHalfSphere(benchmark::State& state) {
    RandomNumberGenerator rng;
    Vector4 v, h(0, 1, 0);
    for (auto _ : state) {
        rng.uniformSurfaceUnitHalfSphere(h, v);
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(RandomSurfaceUnitHalfSphere);

static void RandomUniformConeDirection(benchmark::State& state) {
    RandomNumberGenerator rng;
    Vector4 v, d(0, 1, 0);
    float angle = 0.1;
    for (auto _ : state) {
        rng.uniformConeDirection(d, angle, v);
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(RandomUniformConeDirection);

static void RandomUniformTriangle3DBary(benchmark::State& state) {
    RandomNumberGenerator rng;
    BarycentricCoordinate bary;
    for (auto _ : state) {
        rng.uniformTriangle3D(bary);
        benchmark::DoNotOptimize(bary);
    }
}
BENCHMARK(RandomUniformTriangle3DBary);

static void RandomUniformTriangle3DVec(benchmark::State& state) {
    RandomNumberGenerator rng;
    Vector4 v1(2,0,0), v2(3,4,5), v3(-2,3,-4);
    Vector4 v;
    float angle = 0.1;
    for (auto _ : state) {
        rng.uniformTriangle3D(v1,v2,v3,v);
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(RandomUniformTriangle3DVec);



BENCHMARK_MAIN();

