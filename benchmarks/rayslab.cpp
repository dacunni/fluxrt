#include <benchmark/benchmark.h>
#include <random>
#include "slab.h"

static void RaySlabIntersects1(benchmark::State& state) {
    Slab obj(-1.0f, -2.0f, -3.0f, 1.0f, 2.0f, 3.0f);
    Ray ray(Position3(0.1, 0.4, 0.1), Direction3(1.0, 0.2, 0.1).normalized());
    float minDistance = 0.01f;
    benchmark::DoNotOptimize(obj);
    benchmark::DoNotOptimize(ray);
    benchmark::DoNotOptimize(minDistance);
    int numTests = 0;
    for (auto _ : state) {
        benchmark::DoNotOptimize(intersects(ray, obj, minDistance));
        numTests++;
    }
    state.counters["tests"] = benchmark::Counter(numTests, benchmark::Counter::kAvgThreadsRate);
}
BENCHMARK(RaySlabIntersects1);

static void RaySlabIntersectsFirstHit(benchmark::State& state) {
    Slab obj(-1.0f, -2.0f, -3.0f, 1.0f, 2.0f, 3.0f);
    Ray ray(Position3(-5.0, 0.0, 0.0), Direction3(1.0, 0.0, 0.0).normalized());
    float minDistance = 0.01f;
    benchmark::DoNotOptimize(obj);
    benchmark::DoNotOptimize(ray);
    benchmark::DoNotOptimize(minDistance);
    int numTests = 0;
    for (auto _ : state) {
        benchmark::DoNotOptimize(intersects(ray, obj, minDistance));
        numTests++;
    }
    state.counters["tests"] = benchmark::Counter(numTests, benchmark::Counter::kAvgThreadsRate);
}
BENCHMARK(RaySlabIntersectsFirstHit);

static void RaySlabIntersectsSecondHit(benchmark::State& state) {
    Slab obj(-1.0f, -2.0f, -3.0f, 1.0f, 2.0f, 3.0f);
    Ray ray(Position3(0.0, 0.0, 0.0), Direction3(1.0, 0.0, 0.0).normalized());
    float minDistance = 0.01f;
    benchmark::DoNotOptimize(obj);
    benchmark::DoNotOptimize(ray);
    benchmark::DoNotOptimize(minDistance);
    int numTests = 0;
    for (auto _ : state) {
        benchmark::DoNotOptimize(intersects(ray, obj, minDistance));
        numTests++;
    }
    state.counters["tests"] = benchmark::Counter(numTests, benchmark::Counter::kAvgThreadsRate);
}
BENCHMARK(RaySlabIntersectsSecondHit);

static void RaySlabFindIntersection1(benchmark::State& state) {
    Slab obj(-1.0f, -2.0f, -3.0f, 1.0f, 2.0f, 3.0f);
    Ray ray(Position3(0.1, 0.4, 0.1), Direction3(1.0, 0.2, 0.1).normalized());
    RayIntersection intersection;
    float minDistance = 0.01f;
    benchmark::DoNotOptimize(obj);
    benchmark::DoNotOptimize(ray);
    benchmark::DoNotOptimize(intersection);
    benchmark::DoNotOptimize(minDistance);
    int numTests = 0;
    for (auto _ : state) {
        benchmark::DoNotOptimize(findIntersection(ray, obj, minDistance, intersection));
        numTests++;
    }
    state.counters["tests"] = benchmark::Counter(numTests, benchmark::Counter::kAvgThreadsRate);
}
BENCHMARK(RaySlabFindIntersection1);

static void RaySlabFindIntersectionFirstHit(benchmark::State& state) {
    Slab obj(-1.0f, -2.0f, -3.0f, 1.0f, 2.0f, 3.0f);
    Ray ray(Position3(-5.0, 0.0, 0.0), Direction3(1.0, 0.0, 0.0).normalized());
    RayIntersection intersection;
    float minDistance = 0.01f;
    benchmark::DoNotOptimize(obj);
    benchmark::DoNotOptimize(ray);
    benchmark::DoNotOptimize(intersection);
    benchmark::DoNotOptimize(minDistance);
    int numTests = 0;
    for (auto _ : state) {
        benchmark::DoNotOptimize(findIntersection(ray, obj, minDistance, intersection));
        numTests++;
    }
    state.counters["tests"] = benchmark::Counter(numTests, benchmark::Counter::kAvgThreadsRate);
}
BENCHMARK(RaySlabFindIntersectionFirstHit);

static void RaySlabFindIntersectionSecondHit(benchmark::State& state) {
    Slab obj(-1.0f, -2.0f, -3.0f, 1.0f, 2.0f, 3.0f);
    Ray ray(Position3(0.0, 0.0, 0.0), Direction3(1.0, 0.0, 0.0).normalized());
    RayIntersection intersection;
    float minDistance = 0.01f;
    benchmark::DoNotOptimize(obj);
    benchmark::DoNotOptimize(ray);
    benchmark::DoNotOptimize(intersection);
    benchmark::DoNotOptimize(minDistance);
    int numTests = 0;
    for (auto _ : state) {
        benchmark::DoNotOptimize(findIntersection(ray, obj, minDistance, intersection));
        numTests++;
    }
    state.counters["tests"] = benchmark::Counter(numTests, benchmark::Counter::kAvgThreadsRate);
}
BENCHMARK(RaySlabFindIntersectionSecondHit);

BENCHMARK_MAIN();

