#include <benchmark/benchmark.h>
#include "FastRender.h"

static void TransformCreation(benchmark::State& state) {
    for (auto _ : state) {
        benchmark::DoNotOptimize(Transform());
    }
}
BENCHMARK(TransformCreation);

static void TransformCompose2(benchmark::State& state) {
    Transform a, b;
    for (auto _ : state) {
        benchmark::DoNotOptimize(compose(a,b));
    }
}
BENCHMARK(TransformCompose2);

static void TransformCompose3(benchmark::State& state) {
    Transform a, b, c;
    for (auto _ : state) {
        benchmark::DoNotOptimize(compose(a,b,c));
    }
}
BENCHMARK(TransformCompose3);

static void TransformCompose4(benchmark::State& state) {
    Transform a, b, c, d;
    for (auto _ : state) {
        benchmark::DoNotOptimize(compose(a,b,c,d));
    }
}
BENCHMARK(TransformCompose4);

BENCHMARK_MAIN();

