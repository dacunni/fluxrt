#include <benchmark/benchmark.h>
#include <random>
#include "vec3.h"
#include "vec4.h"

static void Vec4DefaultConstruction(benchmark::State& state) {
    for (auto _ : state) {
        vec4 r;
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(Vec4DefaultConstruction);

static void Vec3DefaultConstruction(benchmark::State& state) {
    for (auto _ : state) {
        vec3 r;
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(Vec3DefaultConstruction);

static void Vec4CopyConstruction(benchmark::State& state) {
    vec4 s;
    benchmark::DoNotOptimize(s);
    for (auto _ : state) {
        vec4 r(s);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(Vec4CopyConstruction);

static void Vec3CopyConstruction(benchmark::State& state) {
    vec3 s;
    benchmark::DoNotOptimize(s);
    for (auto _ : state) {
        vec3 r(s);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(Vec3CopyConstruction);

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

static void Vec3Subtract(benchmark::State& state) {
    vec3 a, b, r;
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for (auto _ : state) {
        r = subtract(a, b);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(Vec3Subtract);

static void Vec3Scale(benchmark::State& state) {
    vec3 r;
    float s = 0.0f;
    benchmark::DoNotOptimize(r);
    benchmark::DoNotOptimize(s);
    for (auto _ : state) {
        r = scale(r, s);
        benchmark::DoNotOptimize(r);
        benchmark::DoNotOptimize(s);
    }
}
BENCHMARK(Vec3Scale);

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

BENCHMARK_MAIN();

