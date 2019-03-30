#include <benchmark/benchmark.h>
#include "quaternion.h"

static void QuaternionDefaultConstruction(benchmark::State& state) {
    for (auto _ : state) {
        quaternion q;
        benchmark::DoNotOptimize(q);
    }
}
BENCHMARK(QuaternionDefaultConstruction);

static void QuaternionCopyConstruction(benchmark::State& state) {
    vec3 q;
    benchmark::DoNotOptimize(q);
    for (auto _ : state) {
        vec3 r(q);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(QuaternionCopyConstruction);

static void QuaternionElementConstruction(benchmark::State& state) {
    float x = 3.2, y = 6.4, z = -2.4, w = -1.5;
    benchmark::DoNotOptimize(x);
    benchmark::DoNotOptimize(y);
    benchmark::DoNotOptimize(z);
    benchmark::DoNotOptimize(w);
    for (auto _ : state) {
        quaternion q(x, y, z, w);
        benchmark::DoNotOptimize(q);
    }
}
BENCHMARK(QuaternionElementConstruction);

static void QuaternionVectorScalarConstruction(benchmark::State& state) {
    vec3 v(0.3, 1.8, 0.8);
    float s = 0.23;
    benchmark::DoNotOptimize(v);
    benchmark::DoNotOptimize(s);
    for (auto _ : state) {
        quaternion q(v, s);
        benchmark::DoNotOptimize(q);
    }
}
BENCHMARK(QuaternionVectorScalarConstruction);

static void QuaternionAxisAngleConstruction(benchmark::State& state) {
    vec3 axis(0.3, 1.8, 0.8);
    float angle = 0.23;
    benchmark::DoNotOptimize(axis);
    benchmark::DoNotOptimize(angle);
    for (auto _ : state) {
        quaternion q = quaternion::axisAngle(axis, angle);
        benchmark::DoNotOptimize(q);
    }
}
BENCHMARK(QuaternionAxisAngleConstruction);

static void QuaternionUnitAxisAngleConstruction(benchmark::State& state) {
    vec3 axis(0.3, 1.8, 0.8);
    axis.normalize();
    float angle = 0.23;
    benchmark::DoNotOptimize(axis);
    benchmark::DoNotOptimize(angle);
    for (auto _ : state) {
        quaternion q = quaternion::unitAxisAngle(axis, angle);
        benchmark::DoNotOptimize(q);
    }
}
BENCHMARK(QuaternionUnitAxisAngleConstruction);

static void QuaternionSetElements(benchmark::State& state) {
    float x = 3.2, y = 6.4, z = -2.4, w = -1.5;
    benchmark::DoNotOptimize(x);
    benchmark::DoNotOptimize(y);
    benchmark::DoNotOptimize(z);
    benchmark::DoNotOptimize(w);
    quaternion q;
    for (auto _ : state) {
        q.set(x, y, z, w);
        benchmark::DoNotOptimize(q);
    }
}
BENCHMARK(QuaternionSetElements);

static void QuaternionNorm(benchmark::State& state) {
    quaternion q(2.3, 3.2, 4.4, 1.3);
    benchmark::DoNotOptimize(q);
    for (auto _ : state) {
        float r = norm(q);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(QuaternionNorm);

static void QuaternionConjugate(benchmark::State& state) {
    quaternion q(2.3, 3.2, 4.4, 1.3), r;
    benchmark::DoNotOptimize(q);
    for (auto _ : state) {
        r = conjugate(q);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(QuaternionConjugate);

static void QuaternionMultiplication(benchmark::State& state) {
    quaternion a, b, r;
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for (auto _ : state) {
        r = mult(a, b);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(QuaternionMultiplication);

static void QuaternionRotate(benchmark::State& state) {
    quaternion q(2.3, 3.2, 4.4, 1.3), r;
    vec3 v(3.2, 4.2, -2.4);
    benchmark::DoNotOptimize(q);
    for (auto _ : state) {
        r = rotate(q, v);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(QuaternionRotate);

BENCHMARK_MAIN();

