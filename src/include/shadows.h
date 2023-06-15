#ifndef SHADOWS_H
#define SHADOWS_H

#include <stdint.h>

#define N 8

uint8_t** generateShadows(char *bmpFile, uint8_t k);

void freeShadows(uint8_t **shadows);

#endif