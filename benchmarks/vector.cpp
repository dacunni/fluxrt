#include <benchmark/benchmark.h>
#include <random>
#include "micromath.h"

static void Vec4Creation(benchmark::State& state) {
    for (auto _ : state) {
        vec4 r;
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(Vec4Creation);

static void Vec3Creation(benchmark::State& state) {
    for (auto _ : state) {
        vec3 r;
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(Vec3Creation);

static void Vec4Magnitude(benchmark::State& state) {
    vec4 v(1.3, 4.2, 5.6);
    for (auto _ : state) {
        benchmark::DoNotOptimize(v.magnitude());
    }
}
BENCHMARK(Vec4Magnitude);

static void Vec4MagnitudeSq(benchmark::State& state) {
    vec4 v(1.3, 4.2, 5.6);
    for (auto _ : state) {
        benchmark::DoNotOptimize(v.magnitude_sq());
    }
}
BENCHMARK(Vec4MagnitudeSq);

static void Vec4Normalize(benchmark::State& state) {
    vec4 v(1.3, 4.2, 5.6);
    for (auto _ : state) {
        v.normalize();
        v.x += 1.0f;
    }
}
BENCHMARK(Vec4Normalize);

static void Vec3Normalize(benchmark::State& state) {
    vec3 v(1.3, 4.2, 5.6);

    for (auto _ : state) {
        v.normalize();
        v.x += 1.0f;
    }
}
BENCHMARK(Vec3Normalize);

static void Vec4Normalized(benchmark::State& state) {
    vec4 v(1.3, 4.2, 5.6);
    for (auto _ : state) {
        benchmark::DoNotOptimize(v.normalized());
        v.x += 1.0f;
    }
}
BENCHMARK(Vec4Normalized);

static void Vec3Normalized(benchmark::State& state) {
    vec3 v(1.3, 4.2, 5.6);
    for (auto _ : state) {
        benchmark::DoNotOptimize(v.normalized());
        v.x += 1.0f;
    }
}
BENCHMARK(Vec3Normalized);

static void Vec3Add(benchmark::State& state) {
    vec3 a, b, r;
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for (auto _ : state) {
        r = add(a, b);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(Vec3Add);

static void Vec3Dot(benchmark::State& state) {
    vec3 a, b;
    float r;
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for (auto _ : state) {
        r = dot(a, b);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(Vec3Dot);

static void Vec3Cross(benchmark::State& state) {
    vec3 a, b, r;
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for (auto _ : state) {
        r = cross(a, b);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(Vec3Cross);

static void Vec3Blend(benchmark::State& state) {
    vec3 a, b, r;
    float s = 0.1, t = 0.9;
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for (auto _ : state) {
        r = blend(a, s, b, t);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(Vec3Blend);

static void Vec3Mirror(benchmark::State& state) {
    vec3 a(0.2, 0.2, 0.3), n(0, 1, 0), r;
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(n);
    for (auto _ : state) {
        r = mirror(a, n);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(Vec3Mirror);

static void Vec3Refract(benchmark::State& state) {
    vec3 a(0.2, 0.2, 0.3), n(0, 1, 0);
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(n);
    for (auto _ : state) {
         benchmark::DoNotOptimize(refract(a, n, 1.0, 1.3));
    }
}
BENCHMARK(Vec3Refract);

static void Vec3Interp(benchmark::State& state) {
    vec3 a(0.2, 0.2, 0.3), b(0.5, -0.3, 0.1);
    float alpha = 0.3;
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for (auto _ : state) {
        benchmark::DoNotOptimize(interp(a, b, alpha));
    }
}
BENCHMARK(Vec3Interp);

static void AffineMatrixMult(benchmark::State& state) {
    AffineMatrix xf1 = makeRotationAffine(0.3, vec3(0.2, 0.3, 0.4));
    AffineMatrix xf2 = makeRotationAffine(0.6, vec3(0.9, 0.2, 0.1));
    for (auto _ : state) {
        benchmark::DoNotOptimize(mult(xf1, xf2));
    }
}
BENCHMARK(AffineMatrixMult);

static void AffineMatrixMult2(benchmark::State& state) {
    AffineMatrix xf1 = makeRotationAffine(0.3, vec3(0.2, 0.3, 0.4));
    AffineMatrix xf2 = makeRotationAffine(0.6, vec3(0.9, 0.2, 0.1));
    AffineMatrix r;
    for (auto _ : state) {
        mult(xf1, xf2, r);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(AffineMatrixMult2);

static void AffineMatrixVec4Mult(benchmark::State& state) {
    AffineMatrix xf = makeRotationAffine(0.3, vec3(0.2, 0.3, 0.4));
    vec4 v(2.0, 4.0, 0.3);
    for (auto _ : state) {
        benchmark::DoNotOptimize(mult(xf, v));
    }
}
BENCHMARK(AffineMatrixVec4Mult);

static void AffineMatrixVec4Mult2(benchmark::State& state) {
    AffineMatrix xf = makeRotationAffine(0.3, vec3(0.2, 0.3, 0.4));
    vec4 v(2.0, 4.0, 0.3), r;
    for (auto _ : state) {
        mult(xf, v, r);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(AffineMatrixVec4Mult);

static void AffineMatrixPosition3Mult(benchmark::State& state) {
    AffineMatrix xf = makeRotationAffine(0.3, vec3(0.2, 0.3, 0.4));
    Position3 v(2.0, 4.0, 0.3);
    for (auto _ : state) {
        benchmark::DoNotOptimize(mult(xf, v));
    }
}
BENCHMARK(AffineMatrixPosition3Mult);

static void AffineMatrixPosition3Mult2(benchmark::State& state) {
    AffineMatrix xf = makeRotationAffine(0.3, vec3(0.2, 0.3, 0.4));
    Position3 v(2.0, 4.0, 0.3), r;
    for (auto _ : state) {
        mult(xf, v, r);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(AffineMatrixPosition3Mult);

static void AffineMatrixDirection3Mult(benchmark::State& state) {
    AffineMatrix xf = makeRotationAffine(0.3, vec3(0.2, 0.3, 0.4));
    Direction3 v(2.0, 4.0, 0.3);
    for (auto _ : state) {
        benchmark::DoNotOptimize(mult(xf, v));
    }
}
BENCHMARK(AffineMatrixDirection3Mult);

static void AffineMatrixDirection3Mult2(benchmark::State& state) {
    AffineMatrix xf = makeRotationAffine(0.3, vec3(0.2, 0.3, 0.4));
    Direction3 v(2.0, 4.0, 0.3), r;
    for (auto _ : state) {
        mult(xf, v, r);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(AffineMatrixDirection3Mult);

BENCHMARK_MAIN();

