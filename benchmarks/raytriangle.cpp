#include <benchmark/benchmark.h>
#include <iostream>
#include <vector>
#include "micromath.h"

static void RayTriangleIntersectsAnySingleHit(benchmark::State& state) {
    Ray ray(Position3(0, 0, 10), Direction3(0, 0, -1));
    vec3 v0(-5, -5, 3);
    vec3 v1( 5, -5, 3);
    vec3 v2( 0,  5, 3);
    benchmark::DoNotOptimize(ray);
    benchmark::DoNotOptimize(v0);
    benchmark::DoNotOptimize(v1);
    benchmark::DoNotOptimize(v2);

    for (auto _ : state) {
        bool hit = intersectsTriangle(ray, v0, v1, v2, 0.01);
        benchmark::DoNotOptimize(hit);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(RayTriangleIntersectsAnySingleHit);

static void RayTriangleIntersectsAnySingleHitMinDistFail(benchmark::State& state) {
    Ray ray(Position3(0, 0, 10), Direction3(0, 0, 1));
    vec3 v0(-5, -5, 3);
    vec3 v1( 5, -5, 3);
    vec3 v2( 0,  5, 3);
    benchmark::DoNotOptimize(ray);
    benchmark::DoNotOptimize(v0);
    benchmark::DoNotOptimize(v1);
    benchmark::DoNotOptimize(v2);

    for (auto _ : state) {
        bool hit = intersectsTriangle(ray, v0, v1, v2, 20);
        benchmark::DoNotOptimize(hit);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(RayTriangleIntersectsAnySingleHitMinDistFail);

static void RayTriangleIntersectsAnyMiss(benchmark::State& state) {
    Ray ray(Position3(0, 0, 10), Direction3(0, 1, 0));
    vec3 v0(-5, -5, 3);
    vec3 v1( 5, -5, 3);
    vec3 v2( 0,  5, 3);
    benchmark::DoNotOptimize(ray);
    benchmark::DoNotOptimize(v0);
    benchmark::DoNotOptimize(v1);
    benchmark::DoNotOptimize(v2);

    for (auto _ : state) {
        bool hit = intersectsTriangle(ray, v0, v1, v2, 0.01);
        benchmark::DoNotOptimize(hit);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(RayTriangleIntersectsAnyMiss);

static void RayTriangleArrayIntersectsAnyHitAll(benchmark::State& state) {
    Ray ray(Position3(0, 0, 10), Direction3(0, 0, -1));
    std::vector<vec3> vertices;
    const int nTri = state.range(0);

    for(int i = 0; i < nTri; ++i) {
        float z = -0.1 * float(nTri - i - 1);
        vertices.emplace_back(-5, -5, z);
        vertices.emplace_back( 5, -5, z);
        vertices.emplace_back( 0,  5, z);
    }

    benchmark::DoNotOptimize(ray);
    benchmark::DoNotOptimize(vertices);

    for (auto _ : state) {
        bool hit = intersectsTriangles(ray, &vertices[0], vertices.size(), 0.01);
        benchmark::DoNotOptimize(hit);
    }
    state.SetItemsProcessed(state.iterations() * nTri);
}
BENCHMARK(RayTriangleArrayIntersectsAnyHitAll)
    ->RangeMultiplier(8)->Range(1, 1 << 20);

static void RayTriangleArrayIntersectsAnyMissAll(benchmark::State& state) {
    Ray ray(Position3(0, 0, 10), Direction3(0, 1, 0));
    std::vector<vec3> vertices;
    const int nTri = state.range(0);

    for(int i = 0; i < nTri; ++i) {
        float z = -0.1 * float(nTri - i - 1);
        vertices.emplace_back(-5, -5, z);
        vertices.emplace_back( 5, -5, z);
        vertices.emplace_back( 0,  5, z);
    }

    benchmark::DoNotOptimize(ray);
    benchmark::DoNotOptimize(vertices);

    for (auto _ : state) {
        bool hit = intersectsTriangles(ray, &vertices[0], vertices.size(), 0.01);
        benchmark::DoNotOptimize(hit);
    }
    state.SetItemsProcessed(state.iterations() * nTri);
}
BENCHMARK(RayTriangleArrayIntersectsAnyMissAll)
    ->RangeMultiplier(8)->Range(1, 1 << 20);

static void RayTriangleStripIntersectsAnyMissAll(benchmark::State& state) {
    Ray ray(Position3(0, 0, 10), Direction3(0, 1, 0));
    std::vector<vec3> vertices;
    const int nTri = state.range(0);

    for(int i = 0; i < nTri + 2; ++i) {
        float xoff = 0.1 * floor(i / 2);
        if((i / 2) % 2 == 0) {
            vertices.emplace_back(-5, -5, -5);
        }
        else {
            vertices.emplace_back( 5, -5, 5);
        }
    }

    benchmark::DoNotOptimize(ray);
    benchmark::DoNotOptimize(vertices);

    for (auto _ : state) {
        bool hit = intersectsTriangleStrip(ray, &vertices[0], vertices.size(), 0.01);
        benchmark::DoNotOptimize(hit);
    }
    state.SetItemsProcessed(state.iterations() * nTri);
}
BENCHMARK(RayTriangleStripIntersectsAnyMissAll)
    ->RangeMultiplier(8)->Range(1, 1 << 20);

BENCHMARK_MAIN();

