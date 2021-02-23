#include <stdio.h>
#include "include/sobel.h"

int main() {
    // Creating PGM image structures
    PGMImage source = PGMCreate();
    PGMImage output = PGMCreate();

    // Read PGM image to structure from file
    PGMRead("../images/Aurora8x.pgm", &source);

    // Apply Sobel filter to source image
    // and save result to new PGM image
    Sobel(&source, &output, 8);

    // Write filtered PGM image structure to file
    PGMWrite(&output, "../images/Aurora8x.sobel.pgm");

    // Free allocated memory
    PGMFree(&source);
    PGMFree(&output);

    return 0;
}
