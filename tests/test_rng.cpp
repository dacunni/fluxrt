#include <stdlib.h>
#include <iostream>

#include "base.h"
#include "image.h"
#include "rng.h"

int main(int argc, char ** argv)
{
    int w = 512, h = 256, b = 32;
    float m = std::min(w, h);
    Image<float> image(w, h, 1);
    Image<float> imageRGB(w, h, 3);

    RNG rng;

    int count = 5000;

    image.setAll(0.0);
    for(int i = 0; i < count; ++i) {
        vec2 e = rng.uniformRectangle(0, w, 0, h);
        image.set(e.x, e.y, 0, 1.0);
    }
    writePNG(image, "random_uniformRectangle1.png");

    image.setAll(0.0);
    for(int i = 0; i < count; ++i) {
        vec2 e = rng.uniformRectangle(b, w - b, b, h - b);
        image.set(e.x, e.y, 0, 1.0);
    }
    writePNG(image, "random_uniformRectangle2.png");

    image.setAll(0.0);
    vec2 c = vec2(w / 2, h / 2);
    float r = m / 2 - b;
    for(int i = 0; i < count; ++i) {
        vec2 e = c + rng.uniformCircle(r);
        image.set(e.x, e.y, 0, 1.0);
    }
    writePNG(image, "random_uniformCircle1.png");

    vec3 sc = vec3(w / 2, h / 2, m / 2);
    auto xyz2rgb = [&](vec3 xyz) {
        return (xyz - sc) / (m / 2) + vec3(0.5, 0.5, 0.5);
    };

    imageRGB.setAll(0.0);
    r = m / 2;
    for(int i = 0; i < count; ++i) {
        vec3 e = sc + r * rng.uniformSurfaceUnitSphere();
        vec3 color = xyz2rgb(e);
        imageRGB.set3(e.x, e.y, color.x, color.y, color.z);
    }
    writePNG(imageRGB, "random_uniformSurfaceUnitSphere1.png");

    imageRGB.setAll(0.0);
    r = m / 2;
    for(int i = 0; i < count; ++i) {
        vec3 e = sc + r * rng.uniformSurfaceUnitHalfSphere(Direction3(0.5, 0.5, 0.5));
        vec3 color = xyz2rgb(e);
        imageRGB.set3(e.x, e.y, color.x, color.y, color.z);
    }
    writePNG(imageRGB, "random_uniformSurfaceUnitHalfSphere1.png");

    imageRGB.setAll(0.0);
    r = m / 2;
    for(int i = 0; i < count; ++i) {
        vec3 e = sc + r * rng.uniformSurfaceUnitHalfSphere(Direction3(1.0, 0.0, 0.0));
        vec3 color = xyz2rgb(e);
        imageRGB.set3(e.x, e.y, color.x, color.y, color.z);
    }
    writePNG(imageRGB, "random_uniformSurfaceUnitHalfSphere2.png");

    imageRGB.setAll(0.0);
    r = m / 2;
    for(int i = 0; i < count; ++i) {
        vec3 e = sc + r * rng.uniformSurfaceUnitHalfSphere(Direction3(0.0, 0.0, 1.0));
        vec3 color = xyz2rgb(e);
        imageRGB.set3(e.x, e.y, color.x, color.y, color.z);
    }
    writePNG(imageRGB, "random_uniformSurfaceUnitHalfSphere3.png");

    imageRGB.setAll(0.0);
    r = m / 2;
    for(int i = 0; i < count; ++i) {
        vec3 e = sc + r * rng.cosineAboutDirection(Direction3(0.5, 0.5, 0.5));
        vec3 color = xyz2rgb(e);
        imageRGB.set3(e.x, e.y, color.x, color.y, color.z);
    }
    writePNG(imageRGB, "random_cosineAboutDirection1.png");

    imageRGB.setAll(0.0);
    r = m / 2;
    for(int i = 0; i < count; ++i) {
        vec3 e = sc + r * rng.cosineAboutDirection(Direction3(1.0, 0.0, 0.0));
        vec3 color = xyz2rgb(e);
        imageRGB.set3(e.x, e.y, color.x, color.y, color.z);
    }
    writePNG(imageRGB, "random_cosineAboutDirection2.png");

    imageRGB.setAll(0.0);
    r = m / 2;
    for(int i = 0; i < count; ++i) {
        vec3 e = sc + r * rng.cosineAboutDirection(Direction3(0.0, 0.0, 1.0));
        vec3 color = xyz2rgb(e);
        imageRGB.set3(e.x, e.y, color.x, color.y, color.z);
    }
    writePNG(imageRGB, "random_cosineAboutDirection3.png");

    return EXIT_SUCCESS;
}

