#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <dirent.h>
#include <string.h>
#include <time.h>

#include "./include/shadows.h"
#include "./include/polynomials.h"
#include "./include/modular_arithmetic.h"
#include "./include/bmp.h"

void imageReconstruction(char *bmpFile, uint8_t k, char *bmpDirPath);
void shadowGeneration(char *bmpFile, uint8_t k, char *bmpDirPath);

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s (r|d) <BmpPath> <k> <BmpEmbedDirectory>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *operation = argv[1];
    char *bmpFile = argv[2];
    uint8_t k = atoi(argv[3]);
    char *bmpDirPath = argv[4];

    if (k < 3 || k > 8) {
        fprintf(stderr, "Error: k must be in [3, 8]\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(operation, "r") == 0) {
        imageReconstruction(bmpFile, k, bmpDirPath);
    }
    else if (strcmp(operation, "d") == 0) {
        shadowGeneration(bmpFile, k, bmpDirPath);
    }
    else {
        fprintf(stderr, "Usage: %s (r|d) <BmpPath> <k> <BmpEmbedDirectory>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
}

void imageReconstruction(char *secretImagePath, uint8_t k, char *bmpDirPath) {
    DIR* bmpDir;
    struct dirent* filePath;

    bmpDir = opendir(bmpDirPath);

    if (bmpDir == NULL) {
        fprintf(stderr, "Error while opening directory %s.\n", bmpDirPath);
        exit(EXIT_FAILURE);
    }

    BmpImage *secretImage;
    bool initialized = false;
    uint32_t t; // amount of sub-shadows per shadow

    InsertionMode insertionMode = k < 5 ? LSB4 : LSB2;

    // Matrix where each row contains all the sub-shadows present in one file
    uint8_t **subShadows = malloc(sizeof(uint8_t *) * k);
    uint16_t shadowNumbers[k]; // for each file, the number of shadow it contains
    
    uint32_t fileNumber = 0;

    while ((filePath = readdir(bmpDir)) != NULL && fileNumber < k) {
        if (strcmp(filePath->d_name, ".") != 0 && strcmp(filePath->d_name, "..") != 0) {
            // Build image path
            char path[257];
            snprintf(path, sizeof(path), "%s/%s", bmpDirPath, filePath->d_name);

            // Initialize the secret image with one of the bmp files
            if (!initialized) {
                initialized = true;
                secretImage = bmpRead(path);
                t = (secretImage->header.fileSize - secretImage->header.offset) / (2*k - 2);
            }

            // Extract sub-shadows and shadow number from the bmp file
            uint16_t shadowNumber;
            uint8_t *subShadow = extractSubShadows(path, insertionMode, t, &shadowNumber);

            shadowNumbers[fileNumber] = shadowNumber;
            subShadows[fileNumber++] = subShadow;
        }
    }

    // (2) For each group of vi,1, vi,2, ... , vi,k, i ∈ [1, t], reconstruct fi(x) and gi(x) from mi,1, mi,2, ... , mi,k and di,1, di,2, ... , di,k using Lagrange interpolation.
    Polynomial *f[t];
    Polynomial *g[t];

    uint32_t x[k];
    uint32_t m[k];
    uint32_t d[k];

    for (uint32_t i = 0; i < 2*t; i += 2) { // Iterate over all the sub-shadows
        for (uint32_t j = 0; j < k; j++) {  // for each file
            m[j] = subShadows[j][i];
            d[j] = subShadows[j][i+1];
            x[j] = shadowNumbers[j];
        }

        // Reconstruct the polynomials
        f[i/2] = interpolate(x, m, k);
        g[i/2] = interpolate(x, d, k);
    }

    // Recover the secret and dump it to the destination file
    bool cheatingDetected = recoverSecretImage(secretImage, f, g, t, k);

    if (!cheatingDetected)
        bmpWrite(secretImagePath, secretImage);
    else
        printf("Error: cheating detected\n");

    // Free resources
    for (uint32_t i = 0; i < t; i++) {
        freePolynomial(f[i]);
        freePolynomial(g[i]);
    }
    freeSubShadows(subShadows, k);
    bmpFree(secretImage);
    closedir(bmpDir);
}

void shadowGeneration(char *bmpFile, uint8_t k, char *bmpDirPath) {
    srand(time(NULL)); // seed the random number generator

    uint8_t **shadows = generateShadows(bmpFile, k);
    InsertionMode insertionMode = k < 5 ? LSB4 : LSB2;

    DIR* bmpDir;
    struct dirent* filePath;

    bmpDir = opendir(bmpDirPath);

    if (bmpDir == NULL) {
        fprintf(stderr, "Error while opening directory %s.\n", bmpDirPath);
        freeShadows(shadows);
        exit(EXIT_FAILURE);
    }

    uint32_t i = 0;
    while ((filePath = readdir(bmpDir)) != NULL) {
        // Exclude ".", ".."
        if (strcmp(filePath->d_name, ".") != 0 && strcmp(filePath->d_name, "..") != 0) {
            // Build image path
            char path[257];
            snprintf(path, sizeof(path), "%s/%s", bmpDirPath, filePath->d_name);
            
            embedShadow(path, insertionMode, shadows[i], i + 1);
            i++;
        }
    }

    freeShadows(shadows);
    closedir(bmpDir);
}
