#include <stdio.h>

#include "./include/bmp.h"

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
}
