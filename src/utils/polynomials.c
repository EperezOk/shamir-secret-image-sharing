#include <stdlib.h>
#include <math.h>

#include "../include/modular_arithmetic.h"
#include "../include/polynomials.h"

// All operations are done modulo P

Polynomial* newPolynomial(uint8_t degree) {
    Polynomial* polynomial = malloc(sizeof(Polynomial));
    
    polynomial->coefficients = calloc(1, sizeof(uint32_t) * (degree + 1));
    polynomial->degree = degree;

    return polynomial;
}

uint32_t evaluate(Polynomial * polynomial, uint32_t x) {
    // TODO
    return 0;
}

// Lagrange interpolation
Polynomial* interpolate(uint32_t* X, uint32_t* Y, uint8_t k) {
    // TODO
    return NULL;
}

void freePolynomial(Polynomial* p) {
    free(p->coefficients);
    free(p);
}
