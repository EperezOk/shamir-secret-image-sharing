#include <stdio.h>

#include "./include/bmp.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Missing arguments\n");
        return 0;
    }

    BmpImage *image = bmpRead(argv[1]);

    printf("type: %d\n", image->header.type);
    printf("imageSize: %d x %d\n", image->header.width, image->header.height);
    printf("image offset: %d\n", image->header.offset);

    bmpFree(image);
}
