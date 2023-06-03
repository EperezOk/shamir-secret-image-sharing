#include <stdio.h>
#include <stdlib.h>

#include "./include/common.h"
#include "./include/bmp.h"

BmpImage *bmpRead(const char *filename) {
    FILE *file = fopen(filename, "rb"); // Opens file in binary read mode  

    if (file == NULL)
        handleError("[fopen] Error opening BMP file");

    BmpImage *bmpImage = malloc(sizeof(BmpImage));

    // Read BMP header
    if (fread(&bmpImage->header, sizeof(BmpHeader), 1, file) != 1) {
        fclose(file);
        bmpFree(bmpImage);
        handleError("[fread] Error reading BMP header");
    }

    uint32_t pixelSize = bmpImage->header.width * bmpImage->header.height;

    bmpImage->pixels = malloc(sizeof(uint8_t) * pixelSize);

    // Skip to pixel data
    if (fseek(file, bmpImage->header.offset, SEEK_SET) != 0) {
        fclose(file);
        bmpFree(bmpImage);
        handleError("fseek");
    }

    // Read BMP pixel data
    if (fread(bmpImage->pixels, sizeof(uint8_t), pixelSize, file) != pixelSize) {
        fclose(file);
        bmpFree(bmpImage);
        handleError("[fread] Error reading BMP pixels");
    }

    fclose(file);

    return bmpImage;
}

void bmpWrite(const char *filename, BmpImage *image) {
    // TODO
}

void bmpFree(BmpImage *image) {
    free(image->pixels);
    free(image);
}