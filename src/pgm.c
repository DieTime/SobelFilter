#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "../include/pgm.h"

bool PGMRead(const char *path, PGMImage *image) {
    // Reading status
    bool error = 0;

    // Try open image for reading
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "[ERROR] Couldn't open image from \"%s\"\n", path);
        goto error;
    }

    // Check header magic value
    char magic[3];
    if (read(fd, magic, 3) != 3) {
        fprintf(stderr, "[ERROR] Couldn't read image magic\n");
        goto error;
    }
    if (magic[0] != 'P' || magic[1] != PGM_MAGIC_DIGIT) {
        fprintf(stderr, "[ERROR] Incorrect image magic value\n");
        goto error;
    }

    // Get size from header
    char size[64];
    uint8_t size_pos = 0;
    do {
        if (read(fd, size + size_pos, 1) != 1) {
            fprintf(stderr, "[ERROR] Couldn't read image size\n");
            goto error;
        }

        if (size[size_pos] == '\0') {
            fprintf(stderr, "[ERROR] Incorrect image size\n");
            goto error;
        }
    } while (size[size_pos++] != '\n');

    // Parse size and save width and height
    if (sscanf(size, "%zu %zu", &image->width, &image->height) != 2) {
        fprintf(stderr, "[ERROR] Couldn't parse image size\n");
        goto error;
    }

    // Get chroma (max pixel value) from header
    char chroma_str[64];
    uint8_t chroma_pos = 0;
    do {
        if (read(fd, chroma_str + chroma_pos, 1) != 1) {
            fprintf(stderr, "[ERROR] Couldn't read image chroma\n");
            goto error;
        }

        if (chroma_str[chroma_pos] == '\0') {
            fprintf(stderr, "[ERROR] Incorrect image chroma\n");
            goto error;
        }
    } while (chroma_str[chroma_pos++] != '\n');

    // Parse and save chroma
    if (sscanf(chroma_str, "%hhu", &image->chroma) != 1) {
        fprintf(stderr, "[ERROR] Couldn't parse image chroma\n");
        goto error;
    }

    // Calculate bytes count for reading
    size_t bytes_count = image->width * image->height;

    // Allocate memory for pixels
    image->pixels = malloc(bytes_count);
    if (image->pixels == NULL) {
        fprintf(stderr, "[ERROR] Couldn't allocate image pixels\n");
        goto error;
    }

    // Read pixels to PGM structure
    if (read(fd, image->pixels, bytes_count) != bytes_count) {
        fprintf(stderr, "[ERROR] Couldn't read image pixels\n");
        goto free;
    }

    // Log info on success
    fprintf(
        stderr,
        "[INFO] Successfully read \"%s\"\n"
        "       Image size: %zdx%zd\n"
        "       Chroma: %hhu\n",
        path, image->width, image->height, image->chroma
    );
    goto success;

free:
    PGMFree(image);

error:
    error = 1;

success:
    close(fd);
    return error;
}

bool PGMWrite(PGMImage *image, const char *path) {
    // Saving status
    bool error = 0;

    // Prepare image header and get header length for writing
    char header[128];
    size_t header_length = (size_t)sprintf(
         header,
        "P%c\n"
        "%zd %zd\n"
        "%hhu\n",
        PGM_MAGIC_DIGIT, image->width, image->height, image->chroma
    );

    // Try open file for writing
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 644);
    if (fd == -1) {
        fprintf(stderr, "[ERROR] Couldn't open \"%s\" for writing\n", path);
        goto error;
    }

    // Write header to image file
    if (write(fd, header, header_length) != header_length) {
        fprintf(stderr, "[ERROR] Couldn't write image info\n");
        goto error;
    }

    // Calculate bytes count for writing
    size_t bytes_count = image->width * image->height;

    // Write pixels to image file
    if (write(fd, image->pixels, bytes_count) != bytes_count) {
        fprintf(stderr, "[ERROR] Couldn't write image pixels\n");
        goto error;
    }

    // Log info on success
    fprintf(stderr, "[INFO] Successfully write \"%s\"\n", path);
    goto success;

error:
    error = 1;

success:
    close(fd);
    return error;
}

PGMImage PGMCreate() {
    return (PGMImage) {.pixels = NULL, .width = 0, .height = 0, .chroma = 0};
}

void PGMFree(PGMImage *image) {
    if (image->pixels != NULL) {
        free(image->pixels);
        image->pixels = NULL;
    }
}

uint8_t *ImageGet(PGMImage *image, size_t row, size_t col) {
    return image->pixels + row * image->width + col;
}