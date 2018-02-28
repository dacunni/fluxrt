#include <benchmark/benchmark.h>
#include <cmath>

static void StdSqrtFloat(benchmark::State& state) {
    float a = 3.2f;
    for (auto _ : state) {
        float b = std::sqrt(a);
        benchmark::DoNotOptimize(b);
        a += 1.0f;
    }
}
BENCHMARK(StdSqrtFloat);

static void StdSqrtDouble(benchmark::State& state) {
    double a = 3.2;
    for (auto _ : state) {
        float b = std::sqrt(a);
        benchmark::DoNotOptimize(b);
        a += 1.0;
    }
}
BENCHMARK(StdSqrtDouble);

BENCHMARK_MAIN();
