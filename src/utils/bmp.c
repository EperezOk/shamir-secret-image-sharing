#include <stdio.h>
#include <stdlib.h>

#include "../include/common.h"
#include "../include/bmp.h"

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

    // Read BMP metadata
    uint32_t metadataSize = bmpImage->header.offset - sizeof(BmpHeader);
    bmpImage->metadata = malloc(sizeof(uint8_t) * metadataSize);

    if (fread(bmpImage->metadata, metadataSize, 1, file) != 1) {
        fclose(file);
        bmpFree(bmpImage);
        handleError("[fread] Error reading BMP metadata");
    }
    
    // Read BMP pixel data
    uint32_t pixelSize = bmpImage->header.fileSize - bmpImage->header.offset;
    bmpImage->pixels = malloc(sizeof(uint8_t) * pixelSize);

    if (fread(bmpImage->pixels, pixelSize, 1, file) != 1) {
        fclose(file);
        bmpFree(bmpImage);
        handleError("[fread] Error reading BMP pixels");
    }

    fclose(file);

    return bmpImage;
}

void bmpWrite(const char *filename, BmpImage *image) {
    // Open file in binary write mode
    FILE *file = fopen(filename, "wb");

    if (file == NULL)
        handleError("[fopen] Error opening BMP file");
    
    // Write BMP header
    if (fwrite(&image->header, sizeof(BmpHeader), 1, file) != 1) {
        fclose(file);
        handleError("[fwrite] Error writing BMP header");
    }

    // Write BMP metadata
    uint32_t metadataSize = image->header.offset - sizeof(BmpHeader);
    if (fwrite(image->metadata, metadataSize, 1, file) != 1) {
        fclose(file);
        handleError("[fwrite] Error writing BMP metadata");
    }

    // Write BMP pixel data
    uint32_t pixelSize = image->header.fileSize - image->header.offset;
    if (fwrite(image->pixels, pixelSize, 1, file) != 1) {
        fclose(file);
        handleError("[fwrite] Error writing BMP pixels");
    }

    fclose(file);
} 

void bmpFree(BmpImage *image) {
    free(image->metadata);
    free(image->pixels);
    free(image);
}
