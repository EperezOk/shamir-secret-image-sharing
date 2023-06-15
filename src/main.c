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

void imageReconstruction(char *bmpFile, uint8_t k, char *bmpDirectory);
void shadowGeneration(char *bmpFile, uint8_t k, char *bmpDirectory);

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s (r|d) BmpPath k BmpEmbedDirectory \n", argv[0]);
        return 0;
    }

    char *operation = argv[1];
    char *bmpFile = argv[2];
    uint8_t k = atoi(argv[3]);
    char *bmpDirectory = argv[4];

    if (k < 3 || k > 8) {
        printf("error: k must be in [3, 8]\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(operation, "r") == 0)
        imageReconstruction(bmpFile, k, bmpDirectory);
    else if (strcmp(operation, "d") == 0)
        shadowGeneration(bmpFile, k, bmpDirectory);
    else {
        printf("Operation unrecognized.\n");
        exit(EXIT_FAILURE);
    }

}

void imageReconstruction(char *bmpFile, uint8_t k, char *bmpDirectory) {
    DIR* dir;
    struct dirent* filePath;

    dir = opendir(bmpDirectory);

    if (dir == NULL) {
        printf("Error while opening directory %s.\n", bmpDirectory);
        exit(EXIT_FAILURE);
    }

    // Read Image to calculate t
    BmpImage *image = bmpRead(bmpFile);
    uint32_t t = (image->header.fileSize - image->header.offset) / (2*k - 2);
    bmpFree(image);

    InsertionMode insertionMode = k < 5 ? LSB4 : LSB2;
    // subShadows is a matrix where each row contains all the subshadow for one file (subShadows[1][0] is v_0,1[0] = m_0,1; subShadows[1][1] is v_0,1[1] = d_0,1)
    uint8_t **subShadows = malloc(sizeof(uint8_t *) * k);
    uint16_t shadowNumbers[k]; 
    uint32_t fileNumber = 0;
    
    // (1) Extract vi,j = (mi,j, di,j), i = 1, 2, ... , t, j = 1, 2, ... , k from S1, S2, ... , Sk
    while ((filePath = readdir(dir)) != NULL && fileNumber < k) {
        if (strcmp(filePath->d_name, ".") != 0 && strcmp(filePath->d_name, "..") != 0) {
            // Build path
            char path[257];
            snprintf(path, sizeof(path), "%s/%s", bmpDirectory, filePath->d_name);
            uint16_t shadowNumber;
            uint8_t *subShadow = extractSubShadow(path, insertionMode, t, &shadowNumber);
            // for(uint32_t z = 0; z < t; z++) {
            //     printf("%d ",subShadow[z]);
            // }
            shadowNumbers[fileNumber] = shadowNumber;
            subShadows[fileNumber++] = subShadow;
        }
    }

    closedir(dir);

    // (2) For each group of vi,1, vi,2, ... , vi,k, i ∈ [1, t], reconstruct fi(x) and gi(x) from mi,1, mi,2, ... , mi,k and di,1, di,2, ... , di,k using Lagrange interpolation respectively.
    
    Polynomial *f[t];
    Polynomial *g[t];
    for (uint32_t i = 0; i < 2 * t; i += 2) {
        uint32_t x[k];
        uint32_t m[k];
        uint32_t d[k];
        for (uint32_t j = 0; j < k; j++) {
            m[j] = subShadows[j][i];
            d[j] = subShadows[j][i+1];
            x[j] = shadowNumbers[j];
        }

        f[i/2] = interpolate(x, m, k);
        g[i/2] = interpolate(x, d, k);
    }

    // (3) Let ai,0, ai,1, bi,0 and bi,1 be the coefficients of x0 and x in fi(x) and gi(x) respectively

    // TODO: cheating, not sure how to do the division between a_i_j and b_i_j,
    // for (uint32_t i = 0; i < t ; i++) {
    //     uint32_t a_i_0 = f[i]->coefficients[0];
    //     uint32_t b_i_0 = g[i]->coefficients[0];
    //     uint32_t a_i_1 = f[i]->coefficients[1];
    //     uint32_t b_i_1 = g[i]->coefficients[1];

    //     // ri exists only if a_i_0 can be divided by b_i_0 
    //     if (zPos(-b_i_0) % a_i_0 != 0 || zPos(-b_i_1) % a_i_1 != 0) {
    //         printf("CHEATING!\n");
    //         exit(EXIT_FAILURE);
    //     }
    // }

    // (4) Recover 2k-2 blocks
    createImageWithFunctions(bmpFile, f, g, t, k);

    for (uint32_t i = 0; i < t; i++) {
        freePolynomial(f[i]);
        freePolynomial(g[i]);
    }

    for (uint32_t i = 0; i < fileNumber; i++)
        freeSubShadow(subShadows[i]);
    free(subShadows);
}

void shadowGeneration(char *bmpFile, uint8_t k, char *bmpDirectory) {
    srand(time(NULL)); // seed the random number generator

    uint8_t **shadows = generateShadows(bmpFile, k);
    InsertionMode insertionMode = k < 5 ? LSB4 : LSB2;

    DIR* dir;
    struct dirent* filePath;

    dir = opendir(bmpDirectory);

    if (dir == NULL) {
        printf("Error while opening directory %s.\n", bmpDirectory);
        freeShadows(shadows);
        return;
    }

    uint32_t i = 0;
    while ((filePath = readdir(dir)) != NULL) {
        // Exclude ".", ".."
        if (strcmp(filePath->d_name, ".") != 0 && strcmp(filePath->d_name, "..") != 0) {
            // Build path
            char path[257];
            snprintf(path, sizeof(path), "%s/%s", bmpDirectory, filePath->d_name);
            
            printf("Embedding file: %s\n", path);
            embedShadow(path, insertionMode, shadows[i], i + 1);
            i++;
        }
    }

    closedir(dir);
    freeShadows(shadows);

}
