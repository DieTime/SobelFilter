#include <stdio.h>
#include "include/ppm.h"

int main() {
    PPMImage image;

    // Read PPM image to structure from file
    if (PPMRead("../images/Aurora.ppm", &image) != 0) {
        return 1;
    }

    // Write PPM image structure to file
    if (PPMWrite(&image, "../images/Aurora.sobel.ppm") != 0) {
        return 2;
    }

    // Free allocated memory
    PPMFree(&image);

    return 0;
}
