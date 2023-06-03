#ifndef POLYNOMIALS_H
#define POLYNOMIALS_H

#include <stdint.h>

// All operations will be done in Z_p (modulo P)

typedef struct Polynomial {
    uint32_t* coefficients; // coefficients[0] is the constant term
    uint8_t degree; 
} Polynomial;

Polynomial* newPolynomial(uint8_t degree);

uint32_t evaluate(Polynomial *polynomial, uint32_t x);

// Interpolate a polynomial of degree k - 1 through the k points (x[i], y[i])
Polynomial* interpolate(uint32_t* x, uint32_t* y, uint8_t k);

void freePolynomial(Polynomial* p);

#endif
