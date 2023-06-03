#include <stdio.h>

#include "./include/bmp.h"
#include "./include/modular_arithmetic.h"

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
}
