#include <criterion/criterion.h>

#include "../include/polynomials.h"

Polynomial *p1;
Polynomial *p2;
Polynomial *result;

void setup(void) {
    p1 = newPolynomial(2);
    p1->coefficients[0] = 1;
    p1->coefficients[1] = 2;
    p1->coefficients[2] = 3;
    p2 = newPolynomial(1);
    p2->coefficients[0] = 4;
    p2->coefficients[1] = 5;
}

void teardown(void) {
    freePolynomial(p1);
    freePolynomial(p2);
    freePolynomial(result);
}

// Designed to work with prime P=251
TestSuite(polynomials, .init=setup, .fini=teardown);

Test(polynomials, addition) {
    result = polyAdd(p1, p2);

    uint32_t expectedCoefs[] = {5, 7, 3};
    cr_assert_arr_eq(result->coefficients, expectedCoefs, 3);
}

Test(polynomials, multiplication) {
    result = polyMul(p1, p2);

    uint32_t expectedCoefs[] = {4, 13, 22, 15};
    cr_assert_arr_eq(result->coefficients, expectedCoefs, 4);
}

Test(poynomials, interpolation) {
    uint32_t x[] = {1, 5, 2};
    uint32_t y[] = {3, 10, 9};

    result = interpolate(x, y, 3);

    uint32_t expectedCoefs[] = {36, 73, 145};
    cr_assert_arr_eq(result->coefficients, expectedCoefs, 3);
}
