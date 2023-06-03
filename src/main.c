#include <stdio.h>

#include "./include/bmp.h"
#include "./include/modular_arithmetic.h"
#include "./include/polynomials.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Missing arguments\n");
        return 0;
    }

    // Test bmpRead
    BmpImage *image = bmpRead(argv[1]);

    // Test bmpWrite
    bmpWrite("images/test.bmp", image);
    BmpImage *testImg = bmpRead("images/test.bmp");

    bmpFree(testImg);
    bmpFree(image);

    // Test modular arithmetic
    uint32_t a = 127;
    uint32_t b = 210;

    printf("a + b = %d\n", zAdd(a, b));
    printf("a - b = %d\n", zSub(a, b));
    printf("a * b = %d\n", zMul(a, b));
    printf("a / b = %d\n", zDiv(a, b));
    printf("inv(a) = %d\n", inv(a));
    printf("inv(b) = %d\n", inv(b));

    // Test polynomial interpolation: P(x) = x^2 + 1
    uint32_t x[] = {0, 1, 2};
    uint32_t y[] = {1, 2, 5};

    Polynomial *polynomial = interpolate(x, y, 3);

    for (int i = 0; i < polynomial->degree + 1; i++) {
        printf("Coefficient a_%d=%d\n", i, polynomial->coefficients[i]);
    }
    printf("P(1)=%d\n", evaluate(polynomial, 1));
    printf("P(5)=%d\n", evaluate(polynomial, 5));
    printf("P(2)=%d\n", evaluate(polynomial, 2));

    freePolynomial(polynomial);
}
