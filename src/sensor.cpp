#include <cstdio>
#include "sensor.h"

Sensor::Sensor(uint32_t pixelwidth, uint32_t pixelheight)
    : pixelwidth(pixelwidth), pixelheight(pixelheight)
{}

void Sensor::print() const
{
    printf("Sensor: %u x %u\n", pixelwidth, pixelheight);
}

void Sensor::forEachPixel(const PixelFunction & fn)
{
    auto n = nest(range(0u, pixelheight),
                  range(0u, pixelwidth));

    n.for_each<uint32_t, uint32_t>([fn](uint32_t y, uint32_t x) {
        fn(x, y, 0);
    });
}

void Sensor::forEachPixelInRect(const PixelFunction & fn,
                                size_t xmin, size_t ymin,
                                size_t xdim, size_t ydim)
{
    for(int y = ymin; y < ymin + ydim; y++) {
        for(int x = xmin; x < xmin + xdim; x++) {
            fn(x, y, 0);
        }
    }
}

void Sensor::forEachPixelTiled(const PixelFunction & fn, uint32_t tileSize)
{
    // Walk pixels within tile, taking care not to overstep the bounds
    // of the range for imperfect tilings.
    auto walkTile = [&](uint32_t starty, uint32_t startx) {
        auto n = nest(range(starty, std::min(pixelheight, starty + tileSize)),
                      range(startx, std::min(pixelwidth, startx + tileSize)));

        n.for_each<uint32_t, uint32_t>([&](uint32_t y, uint32_t x) {
            fn(x, y, 0);
        });
    };

    auto n = nest(range(0u, tileSize, pixelheight),
                  range(0u, tileSize, pixelwidth));

    n.for_each<uint32_t, uint32_t>(walkTile);
}

void Sensor::forEachPixelThreaded(const PixelFunction & fn, uint32_t numThreads)
{
    auto rowFn = [&](int y, ThreadIndex tid) {
        for(int x = 0; x < pixelwidth; x++) {
            fn(x, y, tid);
        }
    };

    auto threadFn = [&](ThreadIndex tid) {
        for(int y = tid; y < pixelheight; y += numThreads) {
            rowFn(y, tid);
        }
    };

    std::list<std::future<void>> futures;

    for(ThreadIndex tid = 0; tid < numThreads; ++tid) {
        auto fn = [=]() { threadFn(tid); };
        futures.push_back(std::async(std::launch::async, fn));
    }

    for(auto & future : futures) {
        future.wait();
    }
}

void Sensor::forEachPixelTiledThreaded(const PixelFunction & fn, uint32_t tileSize, uint32_t numThreads)
{
    struct Tile {
        uint32_t ymin, xmin, ymax, xmax;
    };

    auto threadFn = [&](ThreadIndex tid) {
        std::vector<Tile> tiles;
        uint32_t counter = 0;

        // Create a list of tiles assigned to this thread
        auto walkTile = [&](uint32_t starty, uint32_t startx) {
            if(counter % numThreads == tid) {
                tiles.push_back({
                    starty, startx,
                    std::min(pixelheight, starty + tileSize),
                    std::min(pixelwidth, startx + tileSize)
                });
            }
            ++counter;
        };

        auto n = nest(range(0u, tileSize, pixelheight),
                      range(0u, tileSize, pixelwidth));

        n.for_each<uint32_t, uint32_t>(walkTile);

        // Walk each tile's pixels in raster order
        for(auto & tile : tiles) {
            auto n = nest(range(tile.ymin, tile.ymax),
                          range(tile.xmin, tile.xmax));

            n.for_each<uint32_t, uint32_t>([&](uint32_t y, uint32_t x) {
                fn(x, y, 0);
            });
        }
    };

    std::list<std::future<void>> futures;

    for(ThreadIndex tid = 0; tid < numThreads; ++tid) {
        auto fn = [=]() { threadFn(tid); };
        futures.push_back(std::async(std::launch::async, fn));
    }

    for(auto & future : futures) {
        future.wait();
    }
}

