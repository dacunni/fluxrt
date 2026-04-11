#ifndef __RENDER_PROJECTION_H__
#define __RENDER_PROJECTION_H__

#include <atomic>
#include <memory>
#include <string>

class Artifacts;
class Renderer;
struct Scene;

struct RenderParams {
    unsigned int numThreads = 1;
    unsigned int samplesPerPixel = 1;
    std::string renderOrder = "tiled";
    std::atomic<bool> * flushImmediate = nullptr;
};

class RenderProjection {
public:
    virtual ~RenderProjection() = default;

    virtual void render(const Scene & scene, Renderer & renderer, const RenderParams & params) = 0;

    static std::unique_ptr<RenderProjection> create(const std::string & name);

protected:
    // Render one view into 'artifacts' with the given camera and pixel dimensions.
    // Results land at pixel (xOffset + px, yOffset + py) in the artifacts image.
    void renderView(const Scene & scene, Renderer & renderer,
                    const class Camera & camera,
                    unsigned int viewW, unsigned int viewH,
                    Artifacts & artifacts,
                    const RenderParams & params,
                    unsigned int xOffset = 0, unsigned int yOffset = 0);
};

// Standard single-view render using scene.camera and scene.sensor
class SingleProjection : public RenderProjection {
public:
    void render(const Scene & scene, Renderer & renderer, const RenderParams & params) override;
};

// Equirectangular (lat-lon) 360 render — output is 2:1 aspect using scene.sensor height
class LatLonProjection : public RenderProjection {
public:
    void render(const Scene & scene, Renderer & renderer, const RenderParams & params) override;
};

// Cubemap cross layout (4N x 3N), N = scene.sensor.pixelwidth
//   Layout:      [+Y]
//           [-X][+Z][+X][-Z]
//                [-Y]
class CubemapProjection : public RenderProjection {
public:
    void render(const Scene & scene, Renderer & renderer, const RenderParams & params) override;
};

#endif
