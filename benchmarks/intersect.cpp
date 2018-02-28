#include <benchmark/benchmark.h>
#include "FastRender.h"

static void IsectAnyRaySphere(benchmark::State& state) {
    RayIntersection isect;
    Ray ray(Vector4(0, 0, -10), Vector4(0, 0, 1));
    Sphere obj;
    float hit;
    for (auto _ : state) {
        hit = obj.intersectsAny(ray, 0.1f);
        benchmark::DoNotOptimize(hit);
    }
}
BENCHMARK(IsectAnyRaySphere);

static void IsectRaySphere(benchmark::State& state) {
    RayIntersection isect;
    Ray ray(Vector4(0, 0, -10), Vector4(0, 0, 1));
    Sphere obj;
    float hit;
    for (auto _ : state) {
        hit = obj.intersect(ray, isect);
        benchmark::DoNotOptimize(hit);
    }
}
BENCHMARK(IsectRaySphere);

static void IsectAnyRayAASlab(benchmark::State& state) {
    RayIntersection isect;
    Ray ray(Vector4(0, 0, -10), Vector4(0, 0, 1));
    AxisAlignedSlab obj(-1, -1, -1, 1, 1, 1);
    float hit;
    for (auto _ : state) {
        hit = obj.intersectsAny(ray, 0.1f);
        benchmark::DoNotOptimize(hit);
    }
}
BENCHMARK(IsectAnyRayAASlab);

static void IsectRayAASlab(benchmark::State& state) {
    RayIntersection isect;
    Ray ray(Vector4(0, 0, -10), Vector4(0, 0, 1));
    AxisAlignedSlab obj(-1, -1, -1, 1, 1, 1);
    float hit;
    for (auto _ : state) {
        hit = obj.intersect(ray, isect);
        benchmark::DoNotOptimize(hit);
    }
}
BENCHMARK(IsectRayAASlab);

BENCHMARK_MAIN();

