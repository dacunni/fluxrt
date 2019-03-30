#include <benchmark/benchmark.h>
#include <random>
#include "matrix.h"

static void AffineMatrixMult(benchmark::State& state) {
    AffineMatrix xf1 = AffineMatrix::rotation(0.3, vec3(0.2, 0.3, 0.4));
    AffineMatrix xf2 = AffineMatrix::rotation(0.6, vec3(0.9, 0.2, 0.1));
    for (auto _ : state) {
        benchmark::DoNotOptimize(mult(xf1, xf2));
    }
}
BENCHMARK(AffineMatrixMult);

static void AffineMatrixMult2(benchmark::State& state) {
    AffineMatrix xf1 = AffineMatrix::rotation(0.3, vec3(0.2, 0.3, 0.4));
    AffineMatrix xf2 = AffineMatrix::rotation(0.6, vec3(0.9, 0.2, 0.1));
    AffineMatrix r;
    for (auto _ : state) {
        mult(xf1, xf2, r);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(AffineMatrixMult2);

static void AffineMatrixMult3(benchmark::State& state) {
    AffineMatrix xf1 = AffineMatrix::rotation(0.3, vec3(0.2, 0.3, 0.4));
    AffineMatrix xf2 = AffineMatrix::rotation(0.6, vec3(0.9, 0.2, 0.1));
    AffineMatrix r;
    for (auto _ : state) {
        r = mult(xf1, xf2);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(AffineMatrixMult3);

static void AffineMatrixVec4Mult(benchmark::State& state) {
    AffineMatrix xf = AffineMatrix::rotation(0.3, vec3(0.2, 0.3, 0.4));
    vec4 v(2.0, 4.0, 0.3);
    for (auto _ : state) {
        benchmark::DoNotOptimize(mult(xf, v));
    }
}
BENCHMARK(AffineMatrixVec4Mult);

static void AffineMatrixVec4Mult2(benchmark::State& state) {
    AffineMatrix xf = AffineMatrix::rotation(0.3, vec3(0.2, 0.3, 0.4));
    vec4 v(2.0, 4.0, 0.3), r;
    for (auto _ : state) {
        mult(xf, v, r);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(AffineMatrixVec4Mult);

static void AffineMatrixPosition3Mult(benchmark::State& state) {
    AffineMatrix xf = AffineMatrix::rotation(0.3, vec3(0.2, 0.3, 0.4));
    Position3 v(2.0, 4.0, 0.3);
    for (auto _ : state) {
        benchmark::DoNotOptimize(mult(xf, v));
    }
}
BENCHMARK(AffineMatrixPosition3Mult);

static void AffineMatrixPosition3Mult2(benchmark::State& state) {
    AffineMatrix xf = AffineMatrix::rotation(0.3, vec3(0.2, 0.3, 0.4));
    Position3 v(2.0, 4.0, 0.3), r;
    for (auto _ : state) {
        mult(xf, v, r);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(AffineMatrixPosition3Mult);

static void AffineMatrixDirection3Mult(benchmark::State& state) {
    AffineMatrix xf = AffineMatrix::rotation(0.3, vec3(0.2, 0.3, 0.4));
    Direction3 v(2.0, 4.0, 0.3);
    for (auto _ : state) {
        benchmark::DoNotOptimize(mult(xf, v));
    }
}
BENCHMARK(AffineMatrixDirection3Mult);

static void AffineMatrixDirection3Mult2(benchmark::State& state) {
    AffineMatrix xf = AffineMatrix::rotation(0.3, vec3(0.2, 0.3, 0.4));
    Direction3 v(2.0, 4.0, 0.3), r;
    for (auto _ : state) {
        mult(xf, v, r);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(AffineMatrixDirection3Mult);

BENCHMARK_MAIN();

