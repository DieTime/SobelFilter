#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "../include/ppm.h"

bool PPMRead(const char *path, PPMImage *image) {
    // Reading status
    bool error = 0;

    // Try open image for reading by path
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "[ERROR] Couldn't open PPM image from \"%s\"\n", path);
        goto error;
    }

    // Check "P6" header magic value
    char magic[3];
    if (read(fd, magic, 3) != 3) {
        fprintf(stderr, "[ERROR] Couldn't read PPM image magic\n");
        goto error;
    }
    if (magic[0] != 'P' || magic[1] != '6') {
        fprintf(stderr, "[ERROR] Incorrect PPM image magic\n");
        goto error;
    }

    // Get size from header
    char size[64];
    uint8_t size_pos = 0;
    do {
        if (read(fd, size + size_pos, 1) != 1) {
            fprintf(stderr, "[ERROR] Couldn't read PPM image size\n");
            goto error;
        }

        if (size[size_pos] == '\0') {
            fprintf(stderr, "[ERROR] Incorrect PPM image size\n");
            goto error;
        }
    } while (size[size_pos++] != '\n');

    // Parse size string and save width and height
    if (sscanf(size, "%zu %zu", &image->width, &image->height) != 2) {
        fprintf(stderr, "[ERROR] Couldn't parse PPM image size\n");
        goto error;
    }

    // Get chroma (max value of red, green or blue pixel) from header
    char chroma_str[64];
    uint8_t chroma_pos = 0;
    do {
        if (read(fd, chroma_str + chroma_pos, 1) != 1) {
            fprintf(stderr, "[ERROR] Couldn't read PPM image color chroma_str\n");
            goto error;
        }

        if (chroma_str[chroma_pos] == '\0') {
            fprintf(stderr, "[ERROR] Incorrect PPM image color chroma_str\n");
            goto error;
        }
    } while (chroma_str[chroma_pos++] != '\n');

    // Parse and save chroma
    if (sscanf(chroma_str, "%hhu", &image->chroma) != 1) {
        fprintf(stderr, "[ERROR] Couldn't parse PPM image chroma\n");
        goto error;
    }

    // Calculate bytes count for reading
    size_t data_bytes = sizeof(PPMPixel) * image->width * image->height;

    // Read and save pixels
    image->data = (PPMPixel *) malloc(data_bytes);
    if (read(fd, image->data, data_bytes) != data_bytes) {
        fprintf(stderr, "[ERROR] Couldn't read PPM image pixels\n");
        goto error;
    }

    // Check pixels for exceeding chroma
    for (size_t i = 0; i < data_bytes / sizeof(PPMPixel); i++) {
        if (image->data[i].r + image->data[i].g + image->data[i].b > image->chroma * 3) {
            fprintf(
                stderr,
                "[ERROR] PPM image pixel has incorrect color values\n"
                "        Pixel #%zu {\n"
                "           r: %hhu\n"
                "           g: %hhu\n"
                "           b: %hhu\n"
                "        }\n"
                "        Chroma value is %hhu\n",
                i, image->data[i].r, image->data[i].g, image->data[i].b, image->chroma
            );
            goto error;
        }
    }

    // Log on success
    fprintf(
        stderr,
        "[INFO] Successfully read \"%s\"\n"
        "       Image size: %zdx%zd\n"
        "       Chroma: %hhu\n",
        path, image->width, image->height, image->chroma
    );
    goto cleanup;

error:
    // Set error return value as 1 on error
    error = 1;

cleanup:
    // Close file descriptor after work
    close(fd);

    // Return error status
    return error;
}

bool PPMWrite(PPMImage *image, const char *path) {
    // Saving status
    bool error = 0;

    // Prepare PPM image header and get header length for writing
    char header[128];
    int header_length = sprintf(
        header,
        "P6\n"
        "%zd %zd\n"
        "%hhu\n",
        image->width, image->height, image->chroma
    );

    // Try open file for writing by path
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 644);
    if (fd == -1) {
        fprintf(stderr, "[ERROR] Couldn't write PPM image to \"%s\"\n", path);
        goto error;
    }

    // Write header to image file
    if (write(fd, header, header_length) != header_length) {
        fprintf(stderr, "[ERROR] Couldn't write PPM image info\n");
        goto error;
    }

    // Check correct of PPM image structure
    if (image->data == NULL) {
        fprintf(
            stderr,
            "[ERROR] Image data is NULL. Maybe you call PPMFree(...) before?\n"
            "        If it is not please add .data field check after PPMRead(...)\n"
        );
        goto error;
    }

    // Write pixels to PPM image file
    size_t data_bytes = sizeof(PPMPixel) * image->width * image->height;
    if (write(fd, image->data, data_bytes) != data_bytes) {
        fprintf(stderr, "[ERROR] Couldn't write PPM image pixels\n");
        goto error;
    }

    // Log on success
    fprintf(stderr, "[INFO] Successfully write \"%s\"\n", path);
    goto cleanup;

error:
    // Set error return value as 1 on error
    error = 1;

cleanup:
    // Close file descriptor after work
    close(fd);

    // Return error status
    return error;
}

void PPMFree(PPMImage *image) {
    // Free allocated data field
    if (image->data != NULL) {
        free(image->data);
    }

    // Set data for null for safety
    image->data = NULL;
}