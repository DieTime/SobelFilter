#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "include/sobel.h"

int main(int argc, char **argv) {
    // Arguments data
    bool src_flag = 0; char *src = "\0";
    bool out_flag = 0; char *out = "\0";
    int jobs = 1;

    // Current arguments token
    int token;

    // Start parsing arguments
    while ((token = getopt(argc, argv, "S:O:J:")) != -1) {
        switch (token) {
            case 'S':
                src_flag = 1;
                src = optarg;
                break;
            case 'O':
                out_flag = 1;
                out = optarg;
                break;
            case 'J':
                if ((jobs = atoi(optarg)) <= 0) {
                    printf("Option -J requires a positive number\n");
                    return 1;
                }
                break;
            case '?':
                if (optopt == 'S' || optopt == 'O' || optopt == 'J') {
                    printf("Option -%c requires an argument.\n", optopt);
                } else {
                    printf("Unknown option `-%c'.\n", optopt);
                }
                return 2;
            default:
                return 3;
        }
    }

    // Check required -S flag
    if (!src_flag) {
        printf("Missing required -S option with argument\n");
        return 4;
    }

    // Check required -O flag
    if (!out_flag) {
        printf("Missing required -O option with argument\n");
        return 5;
    }

    // Log parsed arguments
    printf("Source image: \"%s\"\nOutput image: \"%s\"\nThreads: %d\n", src, out, jobs);

    // Creating PGM image structures
    PGMImage source = PGMCreate();
    PGMImage output = PGMCreate();

    // Read PGM image to structure from file
    PGMRead(src, &source);

    // Apply Sobel filter to source image
    // and save result to new PGM image
    Sobel(&source, &output, (uint8_t)jobs);

    // Write filtered PGM image structure to file
    PGMWrite(&output, out);

    // Free allocated memory
    PGMFree(&source);
    PGMFree(&output);

    return 0;
}