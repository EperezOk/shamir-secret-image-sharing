#ifndef MODULAR_ARITHMETIC_H
#define MODULAR_ARITHMETIC_H

#define P 251 // prime number

#include <stdint.h>

// Define operations on Z_p (modulo P)

// Return a+b (mod P)
uint32_t zAdd(uint32_t a, uint32_t b);

// Return a-b (mod P)
uint32_t zSub(uint32_t a, uint32_t b);

// Return a*b (mod P)
uint32_t zMul(uint32_t a, uint32_t b);

// Return a/b (mod P)
uint32_t zDiv(uint32_t a, uint32_t b);

// Make `a` positive (mod P)
uint32_t zPos(int32_t a);

// Return x so that a*x = 1 (mod P)
uint32_t inv(uint32_t a);

#endif
