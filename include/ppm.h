#ifndef SOBEL_PPM_H
#define SOBEL_PPM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} PPMPixel;

typedef struct {
    PPMPixel *data;
    size_t width;
    size_t height;
    uint8_t chroma;
} PPMImage;

/**
 * Reading ppm image to PPM image structure
 *
 * PPM image standard:
 *
 *      P6
 *      <width> <height>
 *      <chroma>
 *      rgb rgb rgb...
 *      ..............
 *
 * @path Path to PPM image
 * @return PPM image struct, may have NULL data on error
 */
bool PPMRead(const char *path, PPMImage *image);

/**
 * Saving PPM image structure as ppm image
 *
 * @image PPM image structure
 * @path Path for saving image
 *
 * @return Saving status
 */
bool PPMWrite(PPMImage *image, const char *path);

/**
 * Safe freeing allocated memory
 * of PPM structure
 *
 * @image PPM image structure for freeing
 */
void PPMFree(PPMImage *image);

#endif //SOBEL_PPM_H
