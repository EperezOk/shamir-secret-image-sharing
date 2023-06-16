#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/common.h"
#include "../include/bmp.h"
#include "../include/polynomials.h"
#include "../include/modular_arithmetic.h"

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

uint8_t* extractSubShadows(const char *imagePath, InsertionMode insertionMode, uint32_t t, uint16_t *shadowNumber) {
    BmpImage *image = bmpRead(imagePath);

    uint8_t shadowBitIndex = 0;
    uint32_t shadowByteIndex = 0;

    uint8_t *subShadows = calloc(2*t, sizeof(uint8_t));

    uint32_t pixelsUsed = 2*t * (insertionMode == LSB2 ? 4 : 2);

    for (uint32_t i = 0; i < pixelsUsed; i++) {
        if (shadowBitIndex == 8) {
            shadowBitIndex = 0;
            shadowByteIndex++;
        }
        
        if (insertionMode == LSB2) {
            // Get last 2 bits
            image->pixels[i] &= 0x03; // 0x03 = 0b00000011

            // Mask to get the current pair of bits inside a byte
            uint8_t mask = 0x03 << (6 - shadowBitIndex);

            // Set the current pair of bits in the current v_i,j
            subShadows[shadowByteIndex] |= (image->pixels[i] << (6 - shadowBitIndex)) & mask;

            shadowBitIndex += 2;
        } else if (insertionMode == LSB4) {
            // Get last 4 bits
            image->pixels[i] &= 0x0F; // 0x0F = 0b00001111

            // Mask to get the current half of bits inside a byte (11110000 or 00001111)
            uint8_t mask = 0x0F << (4 - shadowBitIndex);

            // Set the current half of bits in the current v_i,j
            subShadows[shadowByteIndex] |= (image->pixels[i] << (4 - shadowBitIndex)) & mask;

            shadowBitIndex += 4;
        }
    }
    
    // Get the shadow number (i.e. the `j` used to evaluate `f` and `g` polynomials when created the shadow Sj for this image)
    *shadowNumber = image->header.reserved1;

    bmpFree(image);

    return subShadows;
}

bool recoverSecretImage(BmpImage *image, Polynomial *f[], Polynomial *g[], uint32_t t, uint8_t k) {
    uint8_t blockSize = 2 * k - 2;
    uint8_t block[blockSize];

    // Reconstruct each of the t blocks B_i
    for (uint32_t i = 0; i < t; i++) {
        // Cheating detection
        uint8_t a0 = f[i]->coefficients[0];
        uint8_t a1 = f[i]->coefficients[1];
        uint8_t b0 = g[i]->coefficients[0];
        uint8_t b1 = g[i]->coefficients[1];

        uint8_t r0 = zDiv(zPos(-b0), a0);
        uint8_t r1 = zDiv(zPos(-b1), a1);

        if (r0 != r1)
            return true; // cheating detected

        // Recover a_i
        for (uint8_t j = 0; j < k; j++)
            block[j] = f[i]->coefficients[j];

        // Recover b_i
        for (uint8_t j = 2; j < k; j++) // ignore b0 and b1
            block[j - 2 + k] = g[i]->coefficients[j];

        // Embed block in image
        memcpy(image->pixels + i*blockSize, block, blockSize);
    }

    return false; // no cheating detected
}
