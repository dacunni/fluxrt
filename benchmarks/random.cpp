#include <benchmark/benchmark.h>
#include "rng.h"

// Reference of C++ RNG Engines

static void Ref_UniformFloat_default(benchmark::State& state) {
    std::random_device device;
    std::default_random_engine engine(device());
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    for (auto _ : state) {
        benchmark::DoNotOptimize(distribution(engine));
    }
}
BENCHMARK(Ref_UniformFloat_default);

static void Ref_UniformFloat_minstd_rand(benchmark::State& state) {
    std::random_device device;
    std::minstd_rand engine(device());
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    for (auto _ : state) {
        benchmark::DoNotOptimize(distribution(engine));
    }
}
BENCHMARK(Ref_UniformFloat_minstd_rand);

static void Ref_UniformFloat_mt19937(benchmark::State& state) {
    std::random_device device;
    std::mt19937 engine(device());
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    for (auto _ : state) {
        benchmark::DoNotOptimize(distribution(engine));
    }
}
BENCHMARK(Ref_UniformFloat_mt19937);

static void Ref_UniformFloat_mt19937_64(benchmark::State& state) {
    std::random_device device;
    std::mt19937_64 engine(device());
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    for (auto _ : state) {
        benchmark::DoNotOptimize(distribution(engine));
    }
}
BENCHMARK(Ref_UniformFloat_mt19937_64);

static void Ref_UniformFloat_ranlux24(benchmark::State& state) {
    std::random_device device;
    std::ranlux24 engine(device());
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    for (auto _ : state) {
        benchmark::DoNotOptimize(distribution(engine));
    }
}
BENCHMARK(Ref_UniformFloat_ranlux24);

static void Ref_UniformFloat_knuth_b(benchmark::State& state) {
    std::random_device device;
    std::knuth_b engine(device());
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    for (auto _ : state) {
        benchmark::DoNotOptimize(distribution(engine));
    }
}
BENCHMARK(Ref_UniformFloat_knuth_b);

static void Ref_UniformFloat_device(benchmark::State& state) {
    std::random_device engine;
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    for (auto _ : state) {
        benchmark::DoNotOptimize(distribution(engine));
    }
}
BENCHMARK(Ref_UniformFloat_device);

// RNG Class

static void RandomUniform01(benchmark::State& state) {
    RNG rng;
    for (auto _ : state) {
        benchmark::DoNotOptimize(rng.uniform01());
    }
}
BENCHMARK(RandomUniform01);

static void RandomUniformRange(benchmark::State& state) {
    RNG rng;
    for (auto _ : state) {
        benchmark::DoNotOptimize(rng.uniformRange(2.4, 7.3));
    }
}
BENCHMARK(RandomUniformRange);

static void RandomUniformUnitCirlce(benchmark::State& state) {
    RNG rng;
    float x, y;
    for (auto _ : state) {
        rng.uniformUnitCircle(x, y);
        benchmark::DoNotOptimize(x);
        benchmark::DoNotOptimize(y);
    }
}
BENCHMARK(RandomUniformUnitCirlce);

static void RandomUniformCirlce(benchmark::State& state) {
    RNG rng;
    float x, y;
    for (auto _ : state) {
        rng.uniformCircle(3.0f, x, y);
        benchmark::DoNotOptimize(x);
        benchmark::DoNotOptimize(y);
    }
}
BENCHMARK(RandomUniformCirlce);

static void RandomSurfaceUnitSphereXYZ(benchmark::State& state) {
    RNG rng;
    float x, y, z;
    for (auto _ : state) {
        rng.uniformSurfaceUnitSphere(x, y, z);
        benchmark::DoNotOptimize(x);
        benchmark::DoNotOptimize(y);
        benchmark::DoNotOptimize(z);
    }
}
BENCHMARK(RandomSurfaceUnitSphereXYZ);

static void RandomSurfaceUnitHalfSphereXYZ(benchmark::State& state) {
    RNG rng;
    Direction3 d, n(0, 1, 0);
    for (auto _ : state) {
        rng.uniformSurfaceUnitHalfSphere(n, d);
        benchmark::DoNotOptimize(n);
        benchmark::DoNotOptimize(d);
    }
}
BENCHMARK(RandomSurfaceUnitHalfSphereXYZ);

static void RandomSurfaceUnitHalfSphereCosineDistributionXYZ(benchmark::State& state) {
    RNG rng;
    Direction3 d, n(0, 1, 0);
    for (auto _ : state) {
        rng.cosineAboutDirection(n, d);
        benchmark::DoNotOptimize(n);
        benchmark::DoNotOptimize(d);
    }
}
BENCHMARK(RandomSurfaceUnitHalfSphereCosineDistributionXYZ);

BENCHMARK_MAIN();

