#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../include/modular_arithmetic.h"
#include "../include/polynomials.h"

// All operations are done in Z_p (modulo P)

Polynomial* newPolynomial(uint8_t degree) {
    Polynomial* polynomial = malloc(sizeof(Polynomial));
    
    // Initialize all coefficients to 0
    polynomial->coefficients = calloc(1, sizeof(uint32_t) * (degree + 1));
    polynomial->degree = degree;

    return polynomial;
}

uint32_t evaluate(Polynomial * polynomial, uint32_t x) {
    uint32_t result = 0;

    for (int i = 0; i < polynomial->degree + 1; i++) {
        result = zAdd(result, zMul(polynomial->coefficients[i], (uint32_t) pow(x, i) % P));
    }

    return result;
}

void mulByScalar(Polynomial* polynomial, uint32_t scalar) {
    for (int i = 0; i < polynomial->degree + 1; i++) {
        polynomial->coefficients[i] = zMul(polynomial->coefficients[i], scalar);
    }
}

Polynomial* polyAdd(Polynomial* p1, Polynomial* p2) {
    uint8_t maxDegree = p1->degree > p2->degree ? p1->degree : p2->degree;

    Polynomial* result = newPolynomial(maxDegree);

    for (int i = 0; i < maxDegree + 1; i++) {
        if (i <= p1->degree)
            result->coefficients[i] = zAdd(result->coefficients[i], p1->coefficients[i]);
        if (i <= p2->degree)
            result->coefficients[i] = zAdd(result->coefficients[i], p2->coefficients[i]);
    }

    return result;
}

Polynomial* polyMul(Polynomial* p1, Polynomial* p2) {
    uint8_t maxDegree = p1->degree + p2->degree;

    Polynomial* result = newPolynomial(maxDegree);

    for (int i = 0; i < p1->degree + 1; i++) {
        for (int j = 0; j < p2->degree + 1; j++) {
            result->coefficients[i + j] = zAdd(result->coefficients[i + j], zMul(p1->coefficients[i], p2->coefficients[j]));
        }
    }

    return result;
}

// Lagrange interpolation
Polynomial* interpolate(uint32_t* X, uint32_t* Y, uint8_t k) {
    Polynomial* poly = newPolynomial(k - 1);

    for (int i = 0; i < k; i++) {
        Polynomial *L = newPolynomial(1);

        // First term of the productory of L_i(x)
        L->coefficients[1] = 1;
        if (i == 0)
            L->coefficients[0] = zPos(-X[1]);
        else
            L->coefficients[0] = zPos(-X[0]);
        mulByScalar(L, zDiv(1, zSub(X[i], X[i == 0 ? 1 : 0])));

        // Rest of the productory of L_i(x)
        for (int j = 0; j < k; j++) {
            if (j != i && j != (i == 0 ? 1 : 0)) {
                Polynomial *new = newPolynomial(1);
                new->coefficients[1] = 1;
                new->coefficients[0] = zPos(-X[j]);
                mulByScalar(new, zDiv(1, zSub(X[i], X[j])));

                Polynomial *tmp = L;
                L = polyMul(L, new);

                freePolynomial(new);
                freePolynomial(tmp);
            }
        }

        // L_i(x) * y_i
        mulByScalar(L, Y[i]);

        Polynomial *tmp = poly;
        // Add L_i(x) * y_i to the P(x) sum
        poly = polyAdd(poly, L);

        freePolynomial(tmp);
        freePolynomial(L);
    }

    return poly;
}

void freePolynomial(Polynomial* p) {
    free(p->coefficients);
    free(p);
}

void printPolynomial(Polynomial* p) {
    for (int i = p->degree; i >= 0; i--) {
        printf("%dx^%d", p->coefficients[i], i);
        if (i > 0) printf(" + ");
    }
    printf("\n");
}
