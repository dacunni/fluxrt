#include "RenderProjection.h"
#include "artifacts.h"
#include "camera.h"
#include "Renderer.h"
#include "rng.h"
#include "scene.h"
#include "sensor.h"
#include "timer.h"

#include <algorithm>
#include <stdexcept>
#include <vector>

std::unique_ptr<RenderProjection> RenderProjection::create(const std::string & name)
{
    if(name == "single") {
        return std::unique_ptr<RenderProjection>(new SingleProjection());
    }
    else if(name == "latlon") {
        return std::unique_ptr<RenderProjection>(new LatLonProjection());
    }
    else if(name == "cubemap") {
        return std::unique_ptr<RenderProjection>(new CubemapProjection());
    }
    throw std::runtime_error("Unknown render projection: " + name);
}

void RenderProjection::renderView(
    const Scene & scene,
    Renderer & renderer,
    const Camera & camera,
    unsigned int viewW, unsigned int viewH,
    Artifacts & artifacts,
    const RenderParams & params,
    unsigned int xOffset, unsigned int yOffset)
{
    const float minDistance = 0.0f;

    std::vector<RNG> rng(params.numThreads);

    std::vector<vec2> jitter(params.samplesPerPixel);
    std::generate(jitter.begin(), jitter.end(),
                  [&]() { return rng[0].gaussian2D(0.5f); });

    Sensor viewSensor(viewW, viewH);

    auto tracePixelRay = [&](size_t px, size_t py, size_t threadIndex, uint32_t sampleIndex) {
        const vec2 pixelCenter = vec2(float(px), float(py)) + vec2(0.5f, 0.5f);
        const vec2 jitteredPixel = pixelCenter + jitter[sampleIndex];
        const auto standardPixel = viewSensor.pixelStandardImageLocation(jitteredPixel);

        const vec2 randomBlurCoord = rng[threadIndex].uniformUnitCircle();
        const auto ray = camera.rayThroughStandardImagePlane(standardPixel, randomBlurCoord);

        const int ax = int(xOffset) + int(px);
        const int ay = int(yOffset) + int(py);

        RayIntersection intersection;
        RadianceRGB pixelRadiance;
        bool hit = renderer.traceCameraRay(scene, rng[threadIndex], ray, minDistance, 1,
                                           { VaccuumMedium }, intersection, pixelRadiance);
        artifacts.accumPixelRadiance(ax, ay, pixelRadiance);
        if(hit) {
            artifacts.setIntersection(ax, ay, minDistance, scene, intersection);
        }
    };

    auto renderPixelAllSamples = [&](size_t px, size_t py, size_t threadIndex) {
        ProcessorTimer pixelTimer = ProcessorTimer::makeRunningTimer();
        for(unsigned int si = 0; si < params.samplesPerPixel; ++si) {
            tracePixelRay(px, py, threadIndex, si);
        }
        artifacts.setTime(int(xOffset) + int(px), int(yOffset) + int(py), pixelTimer.elapsed());

        if(params.flushImmediate && params.flushImmediate->exchange(false)) {
            artifacts.writeAll();
        }
    };

    const uint32_t tileSize = 8;

    if(params.renderOrder == "raster") {
        viewSensor.forEachPixelThreaded(renderPixelAllSamples, params.numThreads);
    }
    else {
        // Default: tiled
        viewSensor.forEachPixelTiledThreaded(renderPixelAllSamples, tileSize, params.numThreads);
    }
}

// ---------------------------------------------------------------------------
// SingleProjection
// ---------------------------------------------------------------------------

void SingleProjection::render(const Scene & scene, Renderer & renderer, const RenderParams & params)
{
    const unsigned int W = scene.sensor.pixelwidth;
    const unsigned int H = scene.sensor.pixelheight;

    Artifacts artifacts(int(W), int(H), "trace_");
    renderView(scene, renderer, *scene.camera, W, H, artifacts, params);
    artifacts.writeAll();
}

// ---------------------------------------------------------------------------
// LatLonProjection
// ---------------------------------------------------------------------------

void LatLonProjection::render(const Scene & scene, Renderer & renderer, const RenderParams & params)
{
    // Output width = 2 * height for a proper 2:1 equirectangular image
    const unsigned int H = scene.sensor.pixelheight;
    const unsigned int W = 2 * H;

    SphericalCamera cam;
    cam.setPositionDirectionUp(scene.camera->position,
                               scene.camera->direction,
                               scene.camera->up);

    Artifacts artifacts(int(W), int(H), "latlon_");
    renderView(scene, renderer, cam, W, H, artifacts, params);
    artifacts.writeAll();
}

// ---------------------------------------------------------------------------
// CubemapProjection
// ---------------------------------------------------------------------------

void CubemapProjection::render(const Scene & scene, Renderer & renderer, const RenderParams & params)
{
    // Face size N derived from sensor width; each face is N x N at 90-degree FOV
    const unsigned int N = scene.sensor.pixelwidth;

    // 4N x 3N total image in cross layout:
    //         col0   col1   col2   col3
    // row0           [+Y]
    // row1    [-X]   [+Z]   [+X]   [-Z]
    // row2           [-Y]
    const unsigned int totalW = 4 * N;
    const unsigned int totalH = 3 * N;

    Artifacts artifacts(int(totalW), int(totalH), "cubemap_");

    const float fov90 = float(DegreesToRadians(90.0));

    // Camera basis from scene camera
    const Direction3 fwd   =  scene.camera->direction;
    const Direction3 rt    =  scene.camera->right;
    const Direction3 up    =  scene.camera->up;
    const Direction3 back  = Direction3(fwd.negated());
    const Direction3 left  = Direction3(rt.negated());
    const Direction3 down  = Direction3(up.negated());

    struct FaceDesc {
        Direction3 dir;
        Direction3 upVec;
        unsigned int xOffset;
        unsigned int yOffset;
    };

    FaceDesc faces[6] = {
        { up,   back,  1*N, 0*N },  // +Y (top)
        { left, up,    0*N, 1*N },  // -X
        { fwd,  up,    1*N, 1*N },  // +Z (front)
        { rt,   up,    2*N, 1*N },  // +X
        { back, up,    3*N, 1*N },  // -Z
        { down, fwd,   1*N, 2*N },  // -Y (bottom)
    };

    for(const auto & face : faces) {
        PinholeCamera cam(fov90, fov90);
        cam.setPositionDirectionUp(scene.camera->position, face.dir, face.upVec);
        renderView(scene, renderer, cam, N, N, artifacts, params, face.xOffset, face.yOffset);
    }

    artifacts.writeAll();
}
