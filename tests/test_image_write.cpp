#include <stdlib.h>
#include <iostream>
#include "micromath.h"

int main(int argc, char ** argv)
{
    int w = 512, h = 256;
    Image<uint8_t> imageU8x3(w, h, 3);
    Image<float> imageFx3(w, h, 3);
    Image<float> imageFx1(w, h, 1);

    imageU8x3.setAll(127);
    imageFx3.setAll(0.5);
    imageFx1.setAll(0.5);

    for(int x = 0; x < w/2; x++) {
        for(int y = 0; y < h/2; y++) {
            imageU8x3.set(x, y, 1, 255);
            imageFx3.set(x, y, 1, 1.0);
            imageFx1.set(x, y, 0, 1.0);
        }
    }

    writePNG(imageU8x3, "test_u8x3.png");
    writePNG(imageFx3, "test_fx3.png");
    writePNG(imageFx1, "test_fx1.png");

    // TODO: why does this segfault?
    //writeHDR(imageFx3, "test_fx3.hdr");
    //writeHDR(imageFx1, "test_fx1.hdr");

    return EXIT_SUCCESS;
}

