#include <stdlib.h>

#include "./include/shadows.h"
#include "./include/bmp.h"
#include "./include/modular_arithmetic.h"
#include "./include/polynomials.h"

uint8_t** generateShadows(char *bmpFile, uint8_t k, uint32_t *t) {   
    BmpImage *image = bmpRead(bmpFile);

    // Number of blocks of size 2k-2 pixels in the image
    *t = (image->header.fileSize - image->header.offset) / (2*k - 2);

    uint8_t subShadows[*t][N][2];

    // Iterate through the blocks
    for (uint32_t i = 0; i < *t; i++) {
        // (2) Generate k-1th degree polynomial f_i(x) for each block B_i in the image
        Polynomial *f = newPolynomial(k - 1);
        // (3) Generate k-1th degree polynomial g_i(x) for each block B_i in the image
        Polynomial *g = newPolynomial(k - 1);
        
        uint8_t ri = (rand() % (P-1)) + 1; // random in [1, 250]
        
        uint32_t blockNumber = i * (2*k - 2);
        for (uint32_t j = 0; j < k; j++) {

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
        shadows[j] = malloc(sizeof(uint8_t) * 2* (*t));

        // Get the subshadows of each block
        for (uint32_t i = 0; i < (*t); i++) {
            shadows[j][2*i] = subShadows[i][j][0];
            shadows[j][2*i+1] = subShadows[i][j][1];
        }
    }

    bmpFree(image);

    return shadows;
}

void freeSubShadows(uint8_t **subShadows, uint8_t k) {
    for (uint32_t i = 0; i < k; i++)
        free(subShadows[i]);
    free(subShadows);
}

void freeShadows(uint8_t **shadows) {
    for (uint32_t i = 0; i < N; i++)
        free(shadows[i]);
    free(shadows);
}