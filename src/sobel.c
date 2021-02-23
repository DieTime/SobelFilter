#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "../include/sobel.h"

#define MAX_OF(x, y) ((x) > (y)) ? (x) : (y)

bool Sobel(PGMImage *source, PGMImage *result) {
    struct timeval stop, start;

    // Start clocking
    gettimeofday(&start, NULL);

    // Check source image size
    if (source->width < 3 || source->height < 3) {
        fprintf(stderr, "[ERROR] Source image have too small size. Minimum size 3x3\n");
        return 1;
    }

    // Clear result image before converting
    PGMFree(result);

    // Setup result PGM structure
    result->width = source->width - 2;
    result->height = source->height - 2;
    result->chroma = 0;

    // Calculate result bytes count
    size_t bytes_count = result->width * result->height;

    // Allocate memory for pixels
    result->pixels = (uint8_t*)malloc(bytes_count);
    if (result->pixels == NULL) {
        fprintf(stderr, "[ERROR] Couldn't allocate image pixels\n");
        return 1;
    }

    // Sobel x and y matrix
    int8_t sobel_dx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int8_t sobel_dy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    // Calculating result pixel
    for (size_t i = 1; i < source->height - 1; i++) {
        for (size_t j = 1; j < source->width - 1; j++) {
            uint8_t gray_dx = (uint8_t)
                (*ImageGet(source, i - 1, j - 1) * sobel_dx[0][0]) +
                (*ImageGet(source, i, j - 1) * sobel_dx[0][1]) +
                (*ImageGet(source, i + 1, j - 1) * sobel_dx[0][2]) +
                (*ImageGet(source, i - 1, j) * sobel_dx[1][0]) +
                (*ImageGet(source, i, j) * sobel_dx[1][1]) +
                (*ImageGet(source, i + 1, j) * sobel_dx[1][2]) +
                (*ImageGet(source, i - 1, j + 1) * sobel_dx[2][0]) +
                (*ImageGet(source, i, j + 1) * sobel_dx[2][1]) +
                (*ImageGet(source, i, j + 1) * sobel_dx[2][2]);

            uint8_t gray_dy = (uint8_t)
                (*ImageGet(source, i - 1, j - 1) * sobel_dy[0][0]) +
                (*ImageGet(source, i, j - 1) * sobel_dy[0][1]) +
                (*ImageGet(source, i + 1, j - 1) * sobel_dy[0][2]) +
                (*ImageGet(source, i - 1, j) * sobel_dy[1][0]) +
                (*ImageGet(source, i, j) * sobel_dy[1][1]) +
                (*ImageGet(source, i + 1, j) * sobel_dy[1][2]) +
                (*ImageGet(source, i - 1, j + 1) * sobel_dy[2][0]) +
                (*ImageGet(source, i, j + 1) * sobel_dy[2][1]) +
                (*ImageGet(source, i, j + 1) * sobel_dy[2][2]);

            // Calculate and save pixel value
            uint8_t *pixel = ImageGet(result, i - 1, j - 1);
            *pixel = (uint8_t)ceil(sqrt(gray_dx * gray_dx + gray_dy * gray_dy));

            // Update chroma
            result->chroma = MAX_OF(result->chroma, *pixel);
        }
    }

    // Stop clocking
    gettimeofday(&stop, NULL);

    // Log info on success
    fprintf(
        stderr,
        "[INFO] Successfully apply filter in %lu µs.\n"
        "       Image size: %zdx%zd\n"
        "       Chroma: %hhu\n",
        (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec,
        result->width, result->height, result->chroma
    );

    return 0;
}
