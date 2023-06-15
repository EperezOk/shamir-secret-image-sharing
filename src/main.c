#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <dirent.h>
#include <string.h>
#include <time.h>

#include "./include/shadows.h"
#include "./include/bmp.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s BmpPath k BmpEmbedDirectory \n", argv[0]);
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
    InsertionMode insertionMode = k < 5 ? LSB4 : LSB2;

    DIR* dir;
    struct dirent* filePath;

    dir = opendir(argv[3]);

    if (dir == NULL) {
        printf("Error while opening directory %s.\n", argv[3]);
        freeShadows(shadows);
        return 0;
    }

    uint32_t i = 0;
    while ((filePath = readdir(dir)) != NULL) {
        // Exclude ".", ".."
        if (strcmp(filePath->d_name, ".") != 0 && strcmp(filePath->d_name, "..") != 0) {
            // Build path
            char path[257];
            snprintf(path, sizeof(path), "%s/%s", argv[3], filePath->d_name);
            
            printf("Embedding file: %s\n", path);
            embedShadow(path, insertionMode, shadows[i], i + 1);
            i++;
        }
    }

    closedir(dir);
    freeShadows(shadows);
}
