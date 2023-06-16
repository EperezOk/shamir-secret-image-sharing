#ifndef SHADOWS_H
#define SHADOWS_H

#include <stdint.h>

#include "./bmp.h"

#define N 8

uint8_t** generateShadows(char *bmpFile, uint8_t k, uint32_t *t);

void freeShadows(uint8_t **shadows);

void freeSubShadows(uint8_t **subShadows, uint8_t k);

#endif