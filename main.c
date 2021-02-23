#include <stdio.h>

#include "include/sobel.h"

int main() {
    // Creating PGM image structures
    PGMImage source = PGMCreate();
    PGMImage output = PGMCreate();

    // Read PGM image to structure from file
    if (PGMRead("../images/Aurora8x.pgm", &source) != 0) {
        return 1;
    }

    // Apply Sobel filter to source image
    if (Sobel(&source, &output) != 0) {
        return 2;
    }

    // Write filtered PGM image structure to file
    if (PGMWrite(&output, "../images/Aurora8x.sobel.pgm") != 0) {
        return 3;
    }

    // Free allocated memory
    PGMFree(&source);
    PGMFree(&output);

    return 0;
}
