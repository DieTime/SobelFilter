#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "../include/sobel.h"

#ifndef NDEBUG
    #define DEBUG_LOG(...) printf(__VA_ARGS__);
#else
    #define DEBUG_LOG(...)
#endif

#define MAX_OF(x, y) ((x) > (y)) ? (x) : (y)

bool Sobel(PGMImage *source, PGMImage *result, uint8_t threads) {
    struct timeval stop, start;

    // Log starting info
    printf( "\nStart applying Sobel filter on %hhu threads:\n", threads);

    // Check source image size
    if (source->pixels == NULL) {
        printf( "\t[ERROR] Source image contains no data\n");
        return 1;
    }

    // Check source image size
    if (source->width < 3 || source->height < 3) {
        printf( "\t[ERROR] Source image have too small size. Minimum size 3x3\n");
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
        printf( "\t[ERROR] Couldn't allocate image pixels\n");
        return 1;
    }

    pthread_t pthreads[threads];
    ThreadData args[threads];

    size_t rows_per_tread = result->height / threads;

    // Start clocking
    gettimeofday(&start, NULL);

    for (uint8_t th = 0; th < threads; th++) {
        // Generate arguments for current thread
        args[th] = (ThreadData){
            .source = source,
            .result = result,
            .start_row = 1 + th * rows_per_tread,
            .end_row = 1 + (th + 1) * rows_per_tread,
            .thread_idx = th + 1,
        };

        // Create and run thread
        if (pthread_create(&(pthreads[th]), NULL, SobelApply, (void*)&args[th]) != 0) {
            PGMFree(result);
            return 1;
        }
    }

    // Wait all threads
    for (uint8_t th = 0; th < threads; th++) {
        pthread_join(pthreads[th], NULL);
    }

    // Stop clocking
    gettimeofday(&stop, NULL);

    // Log info on success
    fprintf(
        stderr,
        "\t[INFO] Successfully apply filter in %lu µs.\n"
        "\t       Image size: %zdx%zd\n"
        "\t       Chroma: %hhu\n",
        (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec,
        result->width, result->height, result->chroma
    );

    return 0;
}

void* SobelApply(void *args) {
    // Get data from arguments
    ThreadData *data = (ThreadData*)args;

    DEBUG_LOG("\t[LOG] Thread #%lu created\n", data->thread_idx)

    // Sobel x and y matrix
    int8_t sobel_dx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int8_t sobel_dy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    // Calculate pixels in the desired range of rows
    for (size_t i = data->start_row; (i < data->end_row) && (i < data->source->height - 1); i++) {
        DEBUG_LOG("\t[LOG] Thread #%lu started work on %lu image row\n", data->thread_idx, i)
        for (size_t j = 1; j < data->source->width - 1; j++) {
            uint8_t gray_dx = (uint8_t)
                (*ImageGet(data->source, i - 1, j - 1) * sobel_dx[0][0]) +
                (*ImageGet(data->source, i, j - 1) * sobel_dx[0][1]) +
                (*ImageGet(data->source, i + 1, j - 1) * sobel_dx[0][2]) +
                (*ImageGet(data->source, i - 1, j) * sobel_dx[1][0]) +
                (*ImageGet(data->source, i, j) * sobel_dx[1][1]) +
                (*ImageGet(data->source, i + 1, j) * sobel_dx[1][2]) +
                (*ImageGet(data->source, i - 1, j + 1) * sobel_dx[2][0]) +
                (*ImageGet(data->source, i, j + 1) * sobel_dx[2][1]) +
                (*ImageGet(data->source, i + 1, j + 1) * sobel_dx[2][2]);

            uint8_t gray_dy = (uint8_t)
                (*ImageGet(data->source, i - 1, j - 1) * sobel_dy[0][0]) +
                (*ImageGet(data->source, i, j - 1) * sobel_dy[0][1]) +
                (*ImageGet(data->source, i + 1, j - 1) * sobel_dy[0][2]) +
                (*ImageGet(data->source, i - 1, j) * sobel_dy[1][0]) +
                (*ImageGet(data->source, i, j) * sobel_dy[1][1]) +
                (*ImageGet(data->source, i + 1, j) * sobel_dy[1][2]) +
                (*ImageGet(data->source, i - 1, j + 1) * sobel_dy[2][0]) +
                (*ImageGet(data->source, i, j + 1) * sobel_dy[2][1]) +
                (*ImageGet(data->source, i + 1, j + 1) * sobel_dy[2][2]);

            // Calculate and save pixel value
            uint8_t *pixel = ImageGet(data->result, i - 1, j - 1);
            *pixel = (uint8_t) ceil(sqrt(gray_dx * gray_dx + gray_dy * gray_dy));

            // Update chroma
            data->result->chroma = MAX_OF(data->result->chroma, *pixel);
        }
    }

    DEBUG_LOG("\t[LOG] Thread #%lu has finished\n", data->thread_idx)

    // Terminate current thread
    pthread_exit(NULL);
}