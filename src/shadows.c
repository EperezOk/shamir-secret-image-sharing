#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>

#include "./include/shadows.h"
#include "./include/bmp.h"
#include "./include/modular_arithmetic.h"
#include "./include/polynomials.h"

uint8_t** generateShadows(char *bmpFile, uint8_t k) {   
    BmpImage *image = bmpRead(bmpFile);

    // Number of blocks of size 2k-2 pixels in the image
    uint32_t t = (image->header.fileSize - image->header.offset) / (2*k - 2);

    uint8_t subShadows[t][N][2];

    // Iterate through the blocks
    for (uint32_t i = 0; i < t; i++) {
        // (2) Generate k-1th degree polynomial f_i(x) for each block B_i in the image
        Polynomial *f = newPolynomial(k - 1);
        // (3) Generate k-1th degree polynomial g_i(x) for each block B_i in the image
        Polynomial *g = newPolynomial(k - 1);
        
        uint8_t ri = (rand() % 251) + 1; // random in [1, 251]
        
        for (uint32_t j = 0; j < k; j++) {
            uint32_t blockNumber = i * (2*k - 2);

            // Get pixels [0, k-1] of each block (ai0, ai1, ... , aik-1)
            f->coefficients[j] = zPos(image->pixels[blockNumber + j]);
            
            if (j == 0 || j == 1)
                // b_i,0 = -ri*a_i,0 ; b_i,1 = -ri*a_i,1
                g->coefficients[j] = zMul(zPos(-ri), f->coefficients[j] == 0 ? 1 : f->coefficients[j]); // avoid a_i,j = 0
            else
                // Get pixels [k, 2k-2) of each block (bi2, bi3, ... , bi2k-3)
                g->coefficients[j] = zPos(image->pixels[blockNumber + (j + k - 2)]);
        }
        
        // (4) Generate sub-shadows v_i,j for each block B_i
        for (uint32_t j = 0; j < N; j++) {
            subShadows[i][j][0] = evaluate(f, j+1);
            subShadows[i][j][1] = evaluate(g, j+1);
        }

        freePolynomial(f);
        freePolynomial(g);
    }
    
    // (5) Generate N shadows S_j = (v1,j, v2,j, ... , vt,j)
    uint8_t **shadows = malloc(sizeof(uint8_t *) * N);

    for (uint32_t j = 0; j < N; j++) {
        shadows[j] = malloc(sizeof(uint8_t) * 2*t);

        // Get the subshadows of each block
        for (uint32_t i = 0; i < t; i++) {
            shadows[j][2*i] = subShadows[i][j][0];
            shadows[j][2*i+1] = subShadows[i][j][1];
        }
    }

    bmpFree(image);

    return shadows;
}

uint8_t* extractSubShadow(char *imagePath, InsertionMode insertionMode, uint32_t t, uint16_t *shadowNumber) {
    // (1) Extract vi,j = (mi,j, di,j), i = 1, 2, ... , t, j = 1, 2, ... , k from S1, S2, ... , Sk
    BmpImage *image = bmpRead(imagePath);
    printf("Reading image: %s \n", imagePath);

    uint8_t shadowBitIndex = 0;
    uint32_t shadowByteIndex = 0;
    uint8_t *subShadows = calloc(2*t, sizeof(uint8_t));
    uint32_t pixelsUsed = 2 * t * (insertionMode == LSB2 ? 4 : 2);
    for (uint32_t i = 0; i < pixelsUsed ; i++) {
        if (shadowBitIndex == 8) {
            shadowBitIndex = 0;
            shadowByteIndex++;
        }
        
        if (insertionMode == LSB2) {
            // Get last 2 bits
            image->pixels[i] &= 0x03; // 0x03 = 0b00000011

            // Mask to get certain pair of bits inside a byte (i.e 00110000 or 00001100)
            uint8_t mask = 0x03 << (6 - shadowBitIndex);

            // Add masked bits to the current v_i,j
            subShadows[shadowByteIndex] |= (image->pixels[i] << (6 - shadowBitIndex)) & mask;
            // printf("%d ",subShadows[shadowByteIndex]);
            shadowBitIndex += 2;
        } else if (insertionMode == LSB4) {
            // Get last 4 bits
            image->pixels[i] &= 0x0F; // 0x0F = 0b00001111

            // Mask to get certain pair of bits inside a byte (i.e 11110000 or 00001111)
            uint8_t mask = 0x0F << (4 - shadowBitIndex);

            // Add masked bits to the current v_i,j
            subShadows[shadowByteIndex] |= (image->pixels[i] << (4 - shadowBitIndex)) & mask;
            shadowBitIndex += 4;
        }
    }
    
    *shadowNumber = image->header.reserved1;

    bmpFree(image);

    return subShadows;
}

void freeSubShadow(uint8_t *subShadows) {
    free(subShadows);
}

void freeShadows(uint8_t **shadows) {
    for (uint32_t i = 0; i < N; i++)
        free(shadows[i]);
    free(shadows);
}