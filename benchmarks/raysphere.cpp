#include <benchmark/benchmark.h>
#include <random>
#include "Sphere.h"

static void RaySphereIntersects1(benchmark::State& state) {
    Sphere obj(Position3(0.3, 0.4, 0.2), 0.6);
    Ray ray(Position3(0.1, 0.4, 0.1), Direction3(1.0, 0.2, 0.1).normalized());
    float minDistance = 0.01f;
    benchmark::DoNotOptimize(obj);
    benchmark::DoNotOptimize(ray);
    benchmark::DoNotOptimize(minDistance);
    int numTests = 0;
    for (auto _ : state) {
        benchmark::DoNotOptimize(intersects(ray, obj, minDistance, 1000.0f));
        numTests++;
    }
    state.counters["tests"] = benchmark::Counter(numTests, benchmark::Counter::kAvgThreadsRate);
}
BENCHMARK(RaySphereIntersects1);

static void RaySphereIntersectsFirstHit(benchmark::State& state) {
    Sphere obj(Position3(0.0, 0.0, 0.0), 1.0);
    Ray ray(Position3(-5.0, 0.0, 0.0), Direction3(1.0, 0.0, 0.0).normalized());
    float minDistance = 0.01f;
    benchmark::DoNotOptimize(obj);
    benchmark::DoNotOptimize(ray);
    benchmark::DoNotOptimize(minDistance);
    int numTests = 0;
    for (auto _ : state) {
        benchmark::DoNotOptimize(intersects(ray, obj, minDistance, 1000.0f));
        numTests++;
    }
    state.counters["tests"] = benchmark::Counter(numTests, benchmark::Counter::kAvgThreadsRate);
}
BENCHMARK(RaySphereIntersectsFirstHit);

static void RaySphereIntersectsSecondHit(benchmark::State& state) {
    Sphere obj(Position3(0.0, 0.0, 0.0), 1.0);
    Ray ray(Position3(0.0, 0.0, 0.0), Direction3(1.0, 0.0, 0.0).normalized());
    float minDistance = 0.01f;
    benchmark::DoNotOptimize(obj);
    benchmark::DoNotOptimize(ray);
    benchmark::DoNotOptimize(minDistance);
    int numTests = 0;
    for (auto _ : state) {
        benchmark::DoNotOptimize(intersects(ray, obj, minDistance, 1000.0f));
        numTests++;
    }
    state.counters["tests"] = benchmark::Counter(numTests, benchmark::Counter::kAvgThreadsRate);
}
BENCHMARK(RaySphereIntersectsSecondHit);

static void RaySphereFindIntersection1(benchmark::State& state) {
    Sphere obj(Position3(0.3, 0.4, 0.2), 0.6);
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
BENCHMARK(RaySphereFindIntersection1);

static void RaySphereFindIntersectionFirstHit(benchmark::State& state) {
    Sphere obj(Position3(0.0, 0.0, 0.0), 1.0);
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
BENCHMARK(RaySphereFindIntersectionFirstHit);

static void RaySphereFindIntersectionSecondHit(benchmark::State& state) {
    Sphere obj(Position3(0.0, 0.0, 0.0), 1.0);
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
BENCHMARK(RaySphereFindIntersectionSecondHit);

BENCHMARK_MAIN();

