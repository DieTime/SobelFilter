#ifndef SOBEL_FILTER_PGM_H
#define SOBEL_FILTER_PGM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PGM_MAGIC_DIGIT '5'

typedef struct {
    uint8_t *pixels;
    uint8_t chroma;
    size_t width;
    size_t height;
} PGMImage;

/**
 * Reading image to PGM structure
 *
 * PGM image standard:
 *
 *      P2
 *      <width> <height>
 *      <chroma>
 *      gray gray gray...
 *      .................
 *
 * @path Path to image
 * @return PGM structure
 */
bool PGMRead(const char *path, PGMImage *image);

/**
 * Saving PGM structure as image
 *
 * @image PGM structure
 * @path Path for saving image
 *
 * @return Saving status
 */
bool PGMWrite(PGMImage *image, const char *path);

/**
 * Creating PGM structure with
 * nullable pixels field
 *
 * @return PGM structure
 */
PGMImage PGMCreate();

/**
 * Safe freeing of PGM structure
 *
 * @image PGM structure for freeing
 */
void PGMFree(PGMImage *image);

/**
 * Getting pixel from image by indices
 *
 * @row First coordinate index
 * @param Second coordinate index
 *
 * @return Pixel pointer
 */
uint8_t *ImageGet(PGMImage *image, size_t row, size_t col);

#endif //SOBEL_FILTER_PGM_H
