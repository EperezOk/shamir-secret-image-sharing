#ifndef SHADOWS_H
#define SHADOWS_H

#include <stdint.h>

#include "./bmp.h"

#define N 8

uint8_t** generateShadows(char *bmpFile, uint8_t k);

void freeShadows(uint8_t **shadows);

uint8_t* extractSubShadow(char *imagePath, InsertionMode insertionMode, uint32_t t, uint16_t *shadowNumber);

void freeSubShadow(uint8_t *subShadows);

#endif