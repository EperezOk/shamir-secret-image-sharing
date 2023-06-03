#include "../include/modular_arithmetic.h"

uint32_t gcdExtended(uint32_t a, uint32_t b, int32_t* x, int32_t* y);

uint32_t zAdd(uint32_t a, uint32_t b) {
    return (a + b) % P; 
}

uint32_t zSub(uint32_t a, uint32_t b) {
    return (a - b + P) % P; // make sure result is positive
}

uint32_t zMul(uint32_t a, uint32_t b) {
    return (a * b) % P;
}

uint32_t inv(uint32_t a) {
    int32_t x, y;
    gcdExtended(a, P, &x, &y); // gcd = a*x + P*y = 1 (mod P)

    // Make sure x is positive
    uint32_t inverse = (x % P + P) % P;
    return inverse;
}

uint32_t zDiv(uint32_t a, uint32_t b) {
    return zMul(a, inv(b));
}

// Return the greatest common divisor of a and b, and the coefficients x and y
// so that ax + by = gcd(a, b)
uint32_t gcdExtended(uint32_t a, uint32_t b, int32_t* x, int32_t* y) {
    if (b == 0) {
        *x = 1;
        *y = 0;
        return a;
    }

    int32_t x1, y1; // store results of recursive call
    uint32_t gcd = gcdExtended(b, a % b, &x1, &y1);

    *x = y1;
    *y = x1 - (a / b) * y1;

    return gcd;
}
