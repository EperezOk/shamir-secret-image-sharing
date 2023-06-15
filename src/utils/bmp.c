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

void embedShadow(const char *imagePath, InsertionMode mode, uint8_t *shadow, uint16_t shadowNumber) {
    BmpImage *image = bmpRead(imagePath);
    
    uint32_t pixelSize = image->header.width * image->header.height;
    uint32_t pixelCount = pixelSize * (image->header.bitCount / 8);
    uint8_t shadowBitIndex = 0;
    uint8_t shadowByteIndex = 0;

    for (uint32_t i = 0; i < pixelCount; i++) {
        if (shadowBitIndex == 8) {
            shadowBitIndex = 0;
            shadowByteIndex++;
        }

        if (mode == LSB2) {
            image->pixels[i] &= 0xFC; // 0xFC = 0b11111100
            // Shift the shadow bits to the right, so that the 2 LSBs are the bits we want to embed
            image->pixels[i] |= (shadow[shadowByteIndex] >> (6-shadowBitIndex)) & 0x03;
            shadowBitIndex += 2;
        } else if (mode == LSB4) {
            image->pixels[i] &= 0xF0; // 0xF0 = 0b11110000
            // Shift the shadow bits to the right, so that the 4 LSBs are the bits we want to embed
            image->pixels[i] |= (shadow[shadowByteIndex] >> (4-shadowBitIndex)) & 0x0F;
            shadowBitIndex += 4;
        }
    }

    image->header.reserved1 = shadowNumber;

    bmpWrite(imagePath, image);

    bmpFree(image);
}
