#include <benchmark/benchmark.h>
#include "micromath.h"

static void SlabConstructDefault(benchmark::State& state) {
    for (auto _ : state) {
        Slab slab;
        benchmark::DoNotOptimize(slab);
    }
}
BENCHMARK(SlabConstructDefault);

static void SlabConstructMinMax(benchmark::State& state) {
    for (auto _ : state) {
        Slab slab(-0.4, 0.6, -0.7, 0.3, -0.5, 0.6);
        benchmark::DoNotOptimize(slab);
    }
}
BENCHMARK(SlabConstructMinMax);

static void SlabConstructMinSize(benchmark::State& state) {
    for (auto _ : state) {
        Slab slab(-0.4, 0.6, -0.7, 0.6);
        benchmark::DoNotOptimize(slab);
    }
}
BENCHMARK(SlabConstructMinSize);

static void SlabVolume(benchmark::State& state) {
    Slab slab(-0.4, 0.6, -0.7, 0.3, -0.5, 0.6);
    benchmark::DoNotOptimize(slab);
    for (auto _ : state) {
        auto v = slab.volume();
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(SlabVolume);

static void SlabXDim(benchmark::State& state) {
    Slab slab(-0.4, 0.6, -0.7, 0.3, -0.5, 0.6);
    benchmark::DoNotOptimize(slab);
    for (auto _ : state) {
        auto v = slab.xdim();
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(SlabXDim);

static void SlabXMid(benchmark::State& state) {
    Slab slab(-0.4, 0.6, -0.7, 0.3, -0.5, 0.6);
    benchmark::DoNotOptimize(slab);
    for (auto _ : state) {
        auto v = slab.xmid();
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(SlabXMid);

static void SlabMaxDim(benchmark::State& state) {
    Slab slab(-0.4, 0.6, -0.7, 0.3, -0.5, 0.6);
    benchmark::DoNotOptimize(slab);
    for (auto _ : state) {
        auto v = slab.maxdim();
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(SlabMaxDim);

static void SlabCorrectMinMax(benchmark::State& state) {
    Slab slab(-0.4, 0.6, -0.7, 0.3, -0.5, 0.6);
    benchmark::DoNotOptimize(slab);
    for (auto _ : state) {
        slab.correctMinMax();
        benchmark::DoNotOptimize(slab);
    }
}
BENCHMARK(SlabCorrectMinMax);

static void SlabMerge(benchmark::State& state) {
    Slab slab1(-0.4, 0.6, -0.7, 0.3, -0.5, 0.6);
    Slab slab2(-0.3, 0.5, -0.8, 0.9, -0.2, 0.9);
    benchmark::DoNotOptimize(slab1);
    benchmark::DoNotOptimize(slab2);
    for (auto _ : state) {
        auto m = merge(slab1, slab2);
        benchmark::DoNotOptimize(m);
    }
}
BENCHMARK(SlabMerge);


BENCHMARK_MAIN();

