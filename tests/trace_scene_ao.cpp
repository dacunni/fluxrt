#include <stdlib.h>
#include <iostream>
#include <vector>

#include "scene.h"
#include "image.h"
#include "interpolation.h"

int main(int argc, char ** argv)
{
    if(argc < 2) {
        std::cerr << "Scene argument required\n";
        return EXIT_FAILURE;
    }

    std::string sceneFile = argv[1];

    Scene scene;

    if(!loadSceneFromFile(scene, sceneFile)) {
        std::cerr << "Error loading scene\n";
        return EXIT_FAILURE;
    }

    printf("Scene loaded successfully\n");

    Image<float> image(scene.sensor.pixelwidth, scene.sensor.pixelheight, 1);
    image.setAll(0.0f);

    float minDistance = 0.01f;

    auto tracePixel = [&](Image<float> & image, size_t x, size_t y) {
        auto standardPixel = scene.sensor.pixelStandardImageLocation(float(x) + 0.5f, float(y) + 0.5f);
        auto ray = scene.camera->rayThroughStandardImagePlane(standardPixel.x, standardPixel.y);
        if(intersects(ray, scene, minDistance)) {
            image.set(x, y, 0, 1.0f);
        }
    };

    image.forEachPixel(tracePixel);
    writePNG(image, "ao.png");

    return EXIT_SUCCESS;
}

