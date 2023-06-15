#include <stdio.h>
#include <assert.h>

#include "./include/bmp.h"
#include "./include/modular_arithmetic.h"
#include "./include/polynomials.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Missing arguments\n");
        return 0;
    }

    // Test BMP
    printf("\n\n----- BMP TESTS -----\n");
    BmpImage *image = bmpRead(argv[1]);

    bmpWrite("images/test.bmp", image);
    BmpImage *testImg = bmpRead("images/test.bmp");

    bmpFree(testImg);
    bmpFree(image);


    // Test modular arithmetic
    printf("\n\n----- MODULAR ARITHMETIC TESTS -----\n");

    uint32_t a = 127;
    uint32_t b = 210;

    printf("a + b = %d\n", zAdd(a, b));
    assert(zAdd(a,b) == 86);
    printf("a - b = %d\n", zSub(a, b));
    printf("a * b = %d\n", zMul(a, b));
    printf("a / b = %d\n", zDiv(a, b));
    printf("inv(a) = %d\n", inv(a));
    printf("inv(b) = %d\n", inv(b));


    // Test polynomials
    printf("\n\n----- POLYNOMIALS TESTS -----\n");

    Polynomial *p1 = newPolynomial(2); // P(x) = 3x^2 + 2x + 1
    p1->coefficients[0] = 1;
    p1->coefficients[1] = 2;
    p1->coefficients[2] = 3;
    Polynomial *p2 = newPolynomial(1); // Q(x) = 5x + 4
    p2->coefficients[0] = 4;
    p2->coefficients[1] = 5;

    printf("p1(x) = ");
    printPolynomial(p1);
    printf("p2(x) = ");
    printPolynomial(p2);

    printf("p3(x) = p1(x) + p2(x) = ");
    // P(x) + Q(x) = 3x^2 + 7x + 5 (mod 11)
    Polynomial *p3 = polyAdd(p1, p2);
    printPolynomial(p3);

    printf("p4(x) = p1(x) x p2(x) = ");
    // P(x) * Q(x) = 15x^3 + 22x^2 + 13x + 4
    // P(x) * Q(x) = 4x^3 + 0x^2 + 2x + 4 (mod 11)
    Polynomial *p4 = polyMul(p1, p2);
    printPolynomial(p4);

    mulByScalar(p4, 3);
    printf("3 * p4(x) = ");
    printPolynomial(p4); // 1x^3 + 0x^2 + 6x + 1 (mod 11)

    uint32_t x[] = {1, 5, 2};
    uint32_t y[] = {3, 10, 9};

    // P(x) = 5x^2 + 2x + 7 (mod 11)
    Polynomial *polynomial = interpolate(x, y, 3);
    printf("P(x) = ");
    printPolynomial(polynomial);

    freePolynomial(polynomial);
    freePolynomial(p1);
    freePolynomial(p2);
    freePolynomial(p3);
    freePolynomial(p4);
}