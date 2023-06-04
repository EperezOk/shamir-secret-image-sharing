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

void mulByScalar(Polynomial* polynomial, uint32_t scalar);

Polynomial* polyAdd(Polynomial* p1, Polynomial* p2);

Polynomial* polyMul(Polynomial* p1, Polynomial* p2);

// Interpolate a polynomial of degree k - 1 through k points (x[i], y[i])
Polynomial* interpolate(uint32_t* x, uint32_t* y, uint8_t k);

void freePolynomial(Polynomial* p);

void printPolynomial(Polynomial* p);

#endif
