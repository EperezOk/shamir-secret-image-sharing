#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "./include/shadows.h"
#include "./include/bmp.h"

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Missing arguments\n");
        return 0;
    }

    srand(time(NULL)); // seed the random number generator

    char *bmpFile = argv[1];
    uint8_t k = atoi(argv[2]);

    if (k < 3 || k > 8) {
        printf("error: k must be in [3, 8]\n");
        exit(EXIT_FAILURE);
    }

    // Embed the shadows in the images
    uint8_t **shadows = generateShadows(bmpFile, k);
    InsertionMode insertionMode = k < 5 ? LSB4 : LSB2;

    printf("Amount of arguments %d\n", argc);
    
    for (uint32_t i = 0; i < N; i++) {
        char *targetImage = argv[3+i];
        printf("Embedding file: %s\n", targetImage);
        embedShadow(targetImage, insertionMode, shadows[i], i + 1);
    }

    freeShadows(shadows);
}
