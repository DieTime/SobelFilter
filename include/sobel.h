#ifndef SOBEL_FILTER_SOBEL_H
#define SOBEL_FILTER_SOBEL_H

#include "pgm.h"

typedef struct {
    PGMImage *source;
    PGMImage *result;
    uint8_t thread_idx;
    size_t start_row;
    size_t end_row;
} ThreadData;

/**
 * Apply Sobel filter and place
 * result to result structure
 *
 * @source Source image PGM structure
 * @result Output PGM structure
 * @threads Number of threads for working
 *
 * @return 1 on error, 0 on success
 */
bool Sobel(PGMImage *source, PGMImage* result, uint8_t threads);

/**
 * Tread callback for applying filter
 * from start_row to end_row row
 *
 * @threadData Data containing source PGM
 * struct, output PGM struct, thread index,
 * start_row row and end_row row
 */
void* SobelApply(void *args);

#endif //SOBEL_FILTER_SOBEL_H
