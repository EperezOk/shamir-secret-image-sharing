#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "./include/shadows.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
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

    uint8_t **shadows = generateShadows(bmpFile, k);
    freeShadows(shadows);
}
